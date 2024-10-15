// C++ System Header
#include <csignal>
#include <iostream>
#include <map>
#include <string>

// Project Header
#include "Counseling.h"
#include "DatabaseManager.h"
#include "Food.h"
#include "Healthcare.h"
#include "Outreach.h"
#include "RouteController.h"
#include "Shelter.h"

// Third-party Header
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include "../external_libraries/Crow/include/crow.h"

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

  // Initialize the DatabaseManager
  DatabaseManager dbManager("mongodb://localhost:27017");

    // Create collections for social welfare initiatives
    dbManager.createCollection("Food");
    dbManager.createCollection("Healthcare");
    dbManager.createCollection("Outreach");
    dbManager.createCollection("Shelter");
    dbManager.createCollection("Counseling");
    Shelter s(dbManager);
    s.addShelter("tmp","poor","New York",20,10);
    s.searchShelter();
    // // Initialize the HTTP server
    // crow::SimpleApp app;

    // // Initialize route controller with DatabaseManager
    // RouteController routeController(dbManager);
    // routeController.initRoutes(app);  // Pass the DatabaseManager to the RouteController

    // // Start the server
    // app.port(8080).multithreaded().run();

  return 0;
}
