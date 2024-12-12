#!/bin/bash

set -e  # Exit immediately if a command exits with a non-zero status.

# Set BASE_PATH
BASE_PATH="$(pwd)"

# Define directories
BUILD_DIR="$BASE_PATH/build"
EXTERNAL_LIBRARIES_DIR="$BASE_PATH/external_libraries"

# Install system dependencies
echo "Installing system dependencies..."
sudo apt-get update
sudo apt-get install -y \
    libmongoc-1.0-0 \
    curl \
    cmake \
    build-essential \
    libssl-dev \
    openssl \
    pkg-config \
    libboost-all-dev \
    libpoco-dev \
    libcurl4-openssl-dev \
    docker.io

# Install spdlog
echo "Installing spdlog..."
if [ ! -d "spdlog" ]; then
    git clone https://github.com/gabime/spdlog.git
    cd spdlog
    git checkout v1.12.0
    mkdir build && cd build
    cmake .. -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    sudo make install
    sudo ldconfig
    cd ../..
else
    echo "spdlog already exists, skipping installation."
fi

# Create external_libraries directory
mkdir -p "$EXTERNAL_LIBRARIES_DIR"
cd "$EXTERNAL_LIBRARIES_DIR"

# Clone Crow
echo "Setting up Crow..."
[ ! -d "Crow" ] && git clone --recurse-submodules https://github.com/CrowCpp/Crow.git Crow

# Function to download and extract library
download_and_extract() {
    local name=$1 url=$2 dir=$3 file="${4:-${name}.tar.gz}"
    if [ ! -d "$dir" ]; then
        echo "Downloading $name..."
        curl -L "$url" -o "$file"
        tar -xzf "$file"
        # Get the actual extracted directory name
        extracted_dir=$(tar -tzf "$file" | head -1 | cut -f1 -d"/")
        # Move only if source and destination are different
        if [ "$extracted_dir" != "$dir" ]; then
            mv "$extracted_dir" "$dir"
        fi
        rm "$file"
    else
        echo "$name library already exists, skipping download."
    fi
}

# Download and setup libraries
echo "Setting up Boost..."
download_and_extract "Boost" "https://archives.boost.io/release/1.86.0/source/boost_1_86_0.tar.gz" "boost" "boost_1_86_0.tar.gz"

echo "Setting up Asio..."
download_and_extract "Asio" "https://sourceforge.net/projects/asio/files/asio/1.30.2%20%28Stable%29/asio-1.30.2.tar.gz/download" "asio" "asio-1.30.2.tar.gz"

# MongoDB C++ Driver
echo "Setting up MongoDB C++ Driver..."
if [ ! -d "mongo-cxx-driver" ]; then
    download_and_extract "MongoDB C++ Driver" "https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.11.0/mongo-cxx-driver-r3.11.0.tar.gz" "mongo-cxx-driver" "mongo-cxx-driver-r3.11.0.tar.gz"
    cd mongo-cxx-driver/build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DMONGOCXX_OVERRIDE_DEFAULT_INSTALL_PREFIX=OFF
    sudo cmake --build . --target install
    cd ../..
fi

# JWT-CPP
echo "Setting up jwt-cpp..."
if [ ! -d "jwt-cpp" ]; then
    git clone https://github.com/Thalhammer/jwt-cpp.git
    cd jwt-cpp
    mkdir build && cd build
    cmake ..
    sudo make install
    cd ../..
fi

# BCrypt
echo "Setting up bcrypt..."
if [ ! -d "bcrypt" ]; then
    git clone https://github.com/trusch/libbcrypt.git bcrypt
    cd bcrypt
    mkdir build && cd build
    cmake ..
    make
    sudo make install
    cd ../..
fi

# Poco
echo "Setting up Poco library..."
if [ ! -d "Poco" ]; then
    git clone https://github.com/pocoproject/poco.git Poco
    cd Poco
    mkdir -p cmake-build
    cd cmake-build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc)
    sudo make install
    cd ../..
fi

# Return to base directory and setup build
cd "$BASE_PATH"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "Building the project..."
cmake --build . --config Release

# Setup database
echo "Setting up database..."
docker compose up -d

# Clean up
echo "Cleaning up..."
cd "$EXTERNAL_LIBRARIES_DIR"
rm -f *.tar.gz

echo "Setup complete! 🎉"