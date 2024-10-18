# Git Gud

GitGud is a comprehensive API designed to support various social welfare initiatives, focusing on facilitating access to resources for underserved populations. It serves as a centralized hub for information on diverse initiatives, offering essential services such as shelter, food distribution, healthcare access, outreach programs, and counseling. By connecting individuals and families in need with vital resources, GitGud enhances community support systems and empowers users to navigate their circumstances effectively. Through collaboration with non-profit organizations, volunteers, clinics, and government agencies, GitGud fosters an integrated approach to social welfare that promotes equity and compassion.

# User Types

Our services cater to a diverse range of users, each with unique needs:

- **Homeless (HML)**: Individuals and families without permanent housing.
- **Refugees (RFG)**: Individuals seeking asylum who require immediate shelter and support.
- **Veterans (VET)**: Former military personnel in need of transitional housing and specialized services.
- **Substance Users (SUB)**: Individuals recovering from addiction who require a safe environment and therapeutic support.

## POST/PATCH/DELETE Users:

- **Non-profit Organizations (NGO)**: Collaborating entities that help distribute resources and support.
- **Volunteers (VOL)**: Community members who assist in various service capacities.
- **Clinics (CLN)**: Healthcare facilities that partner with our services to provide medical support.
- **Government (GOV)**: Local and state agencies that help coordinate resources and services.

# External Libraries Installation

The following libraries need to be installed:

External Libraries:
- **Boost**: A set of C++ libraries designed to enhance the standard C++ library. (https://www.boost.org/)
- **Crow**: A C++ micro web framework. (https://crowcpp.org/master/)
- **Asio**: A cross-platform C++ library for network and low-level I/O programming. (https://think-async.com/Asio/)

These libraries are already gathered in `external_libraries/install_libraries.sh`. First, give the script executable permissions with `chmod +x install_libraries.sh`, and then run the script to automatically download and install the required libraries using `./install_libraries.sh`.

brew tap mongodb/brew
brew install mongo-c-driver
brew install mongo-cxx-driver
brew install --cask mongodb-realm-studio

# Running Tests

To build the project, navigate to the build folder and run the following command:

`cmake ..` 
`make`
`make cpplint` (for style checking)

If you ever need to clean up some files `make clean` is always an option

# Running a Cloud Based Instance

# Endpoints
POST /resources/shelter
* Expected Input json:
{ 
 "ORG" : "NGO", 
 "User" : "HML", 
 "location" : "New York", 
 "capacity" : "30", 
 "curUse" : "10" 
}
* Expected Output: status (String)

* Upon Success: HTTP 201 Status Code is returned string Success
* Upon Failure: returned string error msg

GET /resources/shelter
* Expected Output: all data content (String)

* Upon Success: HTTP 200 Status Code is returned string concat all data content
* Upon Failure: returned string error msg
# Branch Coverage

This project uses **GCOV** (coverage tool) and **LCOV** (graphical front-end for GCOV) to generate branch coverage reports for C++ code. After building the project using CMake in the build folder, run `make coverage` which will automatically open the HTML file to view the branch coverage report. If coverage needs to be run again, it may be necessary to clean previous coverage data by using the following commands to delete old `.gcda` and `.gcno` files and rebuild the project:

```bash
find . -name "*.gcda" -delete
find . -name "*.gcno" -delete
```

# Style Checking Analysis

The style checking is done using **cpplint** to ensure compliance with the Google C++. Run the style checker using `make cpplint` to check the source and test files.

# Setup MongoDb Database
1. Install docker
``` bash
# In Linux
sudo snap install docker
```
2. Build the MongoDB locally
The setting is according to [link](https://hub.docker.com/_/mongo/),
``` bash
sudo docker compose up
```
* With this command images( mongo express and mongodb) will pull from dockerhub and build.
* Mongodb IP = http://0.0.0.0:27017, mongo-express=http://0.0.0.0:8081.
* Check more detail in docker-compose.yml file.
## Setup for mongocxx
* Download mongocxx follow the [link](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/get-started/download-and-install/)
* simple test:
It will create the new database and some basic method in database, check test_setup.cpp for detail.
```
c++ --std=c++17 test_setup.cpp -Wl,-rpath,/usr/local/lib/ $(pkg-config --cflags --libs libmongocxx) -o ./app.out
./app.out
```