# COMSW4156-Git-Gud
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