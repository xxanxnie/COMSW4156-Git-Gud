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

# Install external libraries
echo "Starting the download of external libraries..."

cd "$EXTERNAL_LIBRARIES_DIR" || { echo "Failed to change to external libraries directory"; exit 1; }

# Download Crow Library
if [ ! -d "Crow" ]; then
    git clone --recurse-submodules https://github.com/CrowCpp/Crow.git Crow
else
    echo "Crow library already exists, skipping download."
fi

# Download Boost Library
if [ ! -d "boost" ]; then
    curl -L https://archives.boost.io/release/1.86.0/source/boost_1_86_0.tar.gz -o boost_1_86_0.tar.gz
    tar -xzf boost_1_86_0.tar.gz -C .
    mv boost_1_86_0 boost
    rm boost_1_86_0.tar.gz
else
    echo "Boost library already exists, skipping download."
fi

# Download Asio Library
if [ ! -d "asio" ]; then
    curl -L https://sourceforge.net/projects/asio/files/asio/1.30.2%20%28Stable%29/asio-1.30.2.tar.gz/download -o asio-1.30.2.tar.gz
    tar -xzf asio-1.30.2.tar.gz -C .
    mv asio-1.30.2 asio
    rm asio-1.30.2.tar.gz
else
    echo "Asio library already exists, skipping download."
fi

# Download and install MongoDB C++ Driver
if [ ! -d "mongo-cxx-driver" ]; then
    echo "Installing MongoDB C++ Driver..."
    curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.11.0/mongo-cxx-driver-r3.11.0.tar.gz
    tar -xzf mongo-cxx-driver-r3.11.0.tar.gz
    mv mongo-cxx-driver-r3.11.0 mongo-cxx-driver
    cd mongo-cxx-driver/build
    cmake ..                                \
        -DCMAKE_BUILD_TYPE=Release          \
        -DMONGOCXX_OVERRIDE_DEFAULT_INSTALL_PREFIX=OFF
    cmake --build .
    sudo cmake --build . --target install
    cd ../..
    rm mongo-cxx-driver-r3.11.0.tar.gz
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