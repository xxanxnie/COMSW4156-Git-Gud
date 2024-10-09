
The following libraries need to be installed:

External Libraries:
- **Boost**: A set of C++ libraries designed to enhance the standard C++ library. (https://www.boost.org/)
- **Crow**: A C++ micro web framework. (https://crowcpp.org/master/)
- **Asio**: A cross-platform C++ library for network and low-level I/O programming. (https://think-async.com/Asio/)

These libraries are already gathered in `IndividualMiniprojectC++/external_libraries/install_libraries.sh`. First, give the script executable permissions with `chmod +x install_libraries.sh`, and then run the script to automatically download and install the required libraries using `./install_libraries.sh`.

brew tap mongodb/brew
brew install mongo-c-driver
brew install mongo-cxx-driver
brew install --cask mongodb-realm-studio