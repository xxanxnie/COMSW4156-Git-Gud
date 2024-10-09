#ifndef ROUTECONTROLLER_H
#define ROUTECONTROLLER_H

#include "../external_libraries/Crow/include/crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <string>
#include <iostream>

class RouteController {
public:
    // Constructor that accepts the MongoDB URI
    RouteController(const std::string& uri);
    
    // Destructor
    ~RouteController();

    // Method to initialize the HTTP routes
    void initRoutes(crow::App<>& app);

private:
    // Method to initialize the database and collections
    void initializeDatabase();  // Declaration

    // Route handlers
    void index(crow::response& res);
    void getData(crow::response& res);

    // MongoDB client instance
    mongocxx::client mongoClient; 
};

#endif
