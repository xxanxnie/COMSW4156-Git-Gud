#include "include/DatabaseManager.h"
#include <unistd.h> // For sleep
#include <sys/types.h>
#include <sys/wait.h>

MongoDBManager::MongoDBManager(const std::string& dbPath) : dbPath(dbPath), running(false) {}

MongoDBManager::~MongoDBManager() {
    stop();
}

void MongoDBManager::start() {
    if (running) {
        std::cout << "MongoDB is already running." << std::endl;
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        std::string command = "mongod --dbpath " + dbPath;
        execl("/bin/sh", "sh", "-c", command.c_str(), (char*)nullptr);
        exit(0); // Should not reach here if execl is successful
    } else if (pid > 0) {
        // Parent process
        running = true;
        std::cout << "Starting MongoDB..." << std::endl;
        sleep(2); // Give MongoDB time to start
    } else {
        std::cerr << "Failed to fork process." << std::endl;
    }
}

void MongoDBManager::stop() {
    if (!running) return;

    system("pkill mongod");
    running = false;
    std::cout << "MongoDB stopped." << std::endl;
}

bool MongoDBManager::isRunning() const {
    return running;
}
