#ifndef ROUTECONTROLLER_H
#define ROUTECONTROLLER_H

#include <map>
#include <string>
#include <exception>
#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include "../external_libraries/Crow/include/crow.h"
#include "DatabaseManager.h"

class RouteController {
    private:
        DatabaseManager& dbManager;  // Reference to the database manager

    public:
        // Add a constructor
        RouteController(DatabaseManager& dbManager) : dbManager(dbManager) {}

        // Add the initRoutes method declaration
        void initRoutes(crow::SimpleApp& app);

        // Route handlers
        void index(crow::response& res);
        void getResources(const crow::request& req, crow::response& res);
        void addResource(const crow::request& req, crow::response& res);
        void updateResource(const crow::request& req, crow::response& res);
        void deleteResource(const crow::request& req, crow::response& res);

        void addFood(const crow::request& req, crow::response& res);
        void getAllFood(const crow::request& req, crow::response& res);
};

#endif 

