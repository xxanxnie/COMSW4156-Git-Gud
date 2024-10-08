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

# Building and Running a Local Instance

# Running a Cloud Based Instance

# Running Tests

# Endpoints

# Style Checking Analysis




## Setup MongoDb
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