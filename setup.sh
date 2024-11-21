#!/bin/bash

set -e  # Exit immediately if a command exits with a non-zero status.

# Set BASE_PATH
BASE_PATH="$(pwd)"

# Define directories
BUILD_DIR="$BASE_PATH/build"
SRC_DIR="$BASE_PATH/src"
TEST_DIR="$BASE_PATH/test"
EXTERNAL_LIBRARIES_DIR="$BASE_PATH/external_libraries"
COVERAGE_DIR="$BUILD_DIR/coverage"

# Create external_libraries directory if it doesn't exist
mkdir -p "$EXTERNAL_LIBRARIES_DIR"


# Install external libraries
echo "Starting the download of external libraries..."

cd "$EXTERNAL_LIBRARIES_DIR" || { echo "Failed to change to external libraries directory"; exit 1; }

# Function to download and extract library
download_and_extract() {
    local name=$1 url=$2 dir=$3 file="${4:-${name}.tar.gz}"
    if [ ! -d "$dir" ]; then
        echo "Downloading $name..."
        curl -L "$url" -o "$file"
        tar -xzf "$file"
        mv "${dir%-*}" "$dir"
        rm "$file"
    else
        echo "$name library already exists, skipping download."
    fi
}

# Download libraries
[ ! -d "Crow" ] && git clone --recurse-submodules https://github.com/CrowCpp/Crow.git Crow || echo "Crow library already exists, skipping download."

download_and_extract "Boost" "https://archives.boost.io/release/1.86.0/source/boost_1_86_0.tar.gz" "boost" "boost_1_86_0.tar.gz"
download_and_extract "Asio" "https://sourceforge.net/projects/asio/files/asio/1.30.2%20%28Stable%29/asio-1.30.2.tar.gz/download" "asio" "asio-1.30.2.tar.gz"

# Download and install MongoDB C++ Driver
if [ ! -d "mongo-cxx-driver" ]; then
    echo "Installing MongoDB C++ Driver..."
    download_and_extract "MongoDB C++ Driver" "https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.11.0/mongo-cxx-driver-r3.11.0.tar.gz" "mongo-cxx-driver" "mongo-cxx-driver-r3.11.0.tar.gz"
    cd mongo-cxx-driver/build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DMONGOCXX_OVERRIDE_DEFAULT_INSTALL_PREFIX=OFF
    sudo cmake --build . --target install
    cd ../..
else
    echo "MongoDB C++ Driver already exists, skipping installation."
fi

# Download and install Poco library
if [ ! -d "Poco" ]; then
    echo "Downloading and installing Poco library..."
    git clone https://github.com/pocoproject/poco.git Poco
    cd Poco || { echo "Failed to change to Poco directory"; exit 1; }
    mkdir -p cmake-build
    cd cmake-build || { echo "Failed to change to Poco build directory"; exit 1; }
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$EXTERNAL_LIBRARIES_DIR/Poco"
    make -j$(nproc)
    make install
    cd ../../
else
    echo "Poco library already exists, skipping download and installation."
fi

echo "All external libraries processed successfully."

# Return to external libraries directory
cd "$EXTERNAL_LIBRARIES_DIR" || { echo "Failed to return to external libraries directory"; exit 1; }

# Clean up downloaded archives if they exist
for archive in asio-1.30.2.tar.gz boost_1_86_0.tar.gz mongo-cxx-driver-r3.11.0.tar.gz
do
    if [ -f "$archive" ]; then
        rm "$archive"
        echo "Removed $archive"
    else
        echo "$archive not found, skipping removal"
    fi
done

echo "All external libraries downloaded and installed successfully."

# Return to the base directory
cd "$BASE_PATH" || { echo "Failed to return to base directory"; exit 1; }

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

cd "$BUILD_DIR" || { echo "Failed to change to build directory"; exit 1; }
cmake ..

# Compile the project
make -C "$BUILD_DIR" || { echo "Compilation failed"; exit 1; }

sudo apt-get update
sudo apt-get install libmongoc-1.0-0

# Clean up downloaded archives if they exist
for archive in asio-1.30.2.tar.gz boost_1_86_0.tar.gz mongo-cxx-driver-r3.11.0.tar.gz
do
    if [ -f "$archive" ]; then
        rm "$archive"
        echo "Removed $archive"
    else
        echo "$archive not found, skipping removal"
    fi
done

echo "Setup complete!"
