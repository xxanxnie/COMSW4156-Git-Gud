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

# # Clean up
# find "$SRC_DIR" "$TEST_DIR" "$BUILD_DIR" \
#      -type f \( -name '*.o' -o -name '*.gcno' -o -name '*.gcov' -o -name '*.gcda' -o -name '*.info' \) \
#      -delete

# Compile the project
make -C "$BUILD_DIR" || { echo "Compilation failed"; exit 1; }

sudo apt-get update
sudo apt-get install libmongoc-1.0-0

# # Run tests
# "$BUILD_DIR/test_setup" || { echo "Tests failed"; exit 1; }

# # Run gcov
# gcov "$SRC_DIR"/*.cpp

# # Run lcov and generate coverage info
# lcov --capture \
#      --directory "$BASE_PATH" \
#      --output-file "$BUILD_DIR/coverage.info" \
#      --filter range \
#      --ignore-errors inconsistent,empty,unused,path \
#      --include "$SRC_DIR/*" \
#      --exclude "$BASE_PATH/include/*" \

# # Generate HTML report
# genhtml --filter range \
#         --ignore-errors inconsistent \
#         "$BUILD_DIR/coverage.info" \
#         --output-directory "$COVERAGE_DIR"

# # Open the coverage report
# if command -v xdg-open &> /dev/null; then
#     xdg-open "$COVERAGE_DIR/index.html"
# elif command -v open &> /dev/null; then
#     open "$COVERAGE_DIR/index.html"
# else
#     echo "Coverage report generated at $COVERAGE_DIR/index.html"
# fi

# # Clean up
# find "$SRC_DIR" "$TEST_DIR" "$BUILD_DIR" \
#      -type f \( -name '*.o' -o -name '*.gcno' -o -name '*.gcov' -o -name '*.gcda' -o -name '*.info' \) \
#      -delete

# echo "Setup complete. Coverage analysis results saved in $BUILD_DIR/coverage.info"

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
