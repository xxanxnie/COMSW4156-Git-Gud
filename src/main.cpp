// Copyright 2024 COMSW4156-Git-Gud

#include <csignal>
#include <iostream>
#include <map>
#include <string>

#include "Counseling.h"
#include "DatabaseManager.h"
#include "Food.h"
#include "Healthcare.h"
#include "Outreach.h"
#include "RouteController.h"
#include "Shelter.h"

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
  std::signal(SIGINT, signalHandler);
  std::signal(SIGTERM, signalHandler);

  mongocxx::instance instance{};
  DatabaseManager dbManager("mongodb://localhost:27017");

  dbManager.createCollection("Food");
  dbManager.createCollection("Healthcare");
  dbManager.createCollection("Outreach");
  dbManager.createCollection("Shelter");
  dbManager.createCollection("Counseling");

  crow::SimpleApp app;

  RouteController routeController(dbManager);
  routeController.initRoutes(
      app);  
  app.port(8080).multithreaded().run();

  return 0;
}
