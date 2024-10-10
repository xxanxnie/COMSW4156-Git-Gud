#!/bin/bash

echo "Starting the download of external libraries..."

# Download Crow Library
git clone --recurse-submodules https://github.com/CrowCpp/Crow.git Crow

# Download Boost Library
curl -L https://archives.boost.io/release/1.86.0/source/boost_1_86_0.tar.gz -o boost_1_86_0.tar.gz
tar -xzf boost_1_86_0.tar.gz -C .
mv boost_1_86_0 boost

# Download Asio Library
curl -L https://sourceforge.net/projects/asio/files/asio/1.30.2%20%28Stable%29/asio-1.30.2.tar.gz/download -o asio-1.30.2.tar.gz
tar -xzf asio-1.30.2.tar.gz -C .
mv asio-1.30.2 asio

# Remove tar.gz files
rm asio-1.30.2.tar.gz boost_1_86_0.tar.gz

echo "All external libraries downloaded successfully."
