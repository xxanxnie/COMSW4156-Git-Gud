// C++ System Header
#include <iostream>
#include <map>
#include <string>
#include <csignal>

// Project Header
#include "Counseling.h"
#include "Food.h"
#include "Healthcare.h"
#include "Outreach.h"
#include "Shelter.h"
#include "DatabaseManager.h"
#include "RouteController.h"

// Third-party Header
#include "../external_libraries/Crow/include/crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

/**
 *  Method to handle proper termination protocols 
 */
void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "Terminating the application..." << std::endl;
        std::exit(signal);
    }
}

/**
 *  Sets up the HTTP server and runs the program 
 */
int main(int argc, char* argv[]) {
    // Setup signal handling
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // Initialize MongoDB driver
    mongocxx::instance instance{};

    // Initialize the HTTP server
    crow::SimpleApp app;

    // Initialize route controller
    // Create RouteController with the database path and MongoDB URI
    RouteController routeController("/path/to/db", "mongodb://localhost:27017");
    routeController.initRoutes(app);  // Pass the MongoDB database

    // Start the server
    app.port(8080).multithreaded().run();

    return 0;
}