#ifndef MONGODB_MANAGER_H
#define MONGODB_MANAGER_H

#include <string>
#include <iostream>
#include <cstdlib>

class MongoDBManager {
public:
    MongoDBManager(const std::string& dbPath);
    ~MongoDBManager();

    void start();
    void stop();
    bool isRunning() const;

private:
    std::string dbPath;
    bool running;
};

#endif // MONGODB_MANAGER_H
