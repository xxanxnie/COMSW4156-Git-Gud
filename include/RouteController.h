#ifndef ROUTECONTROLLER_H
#define ROUTECONTROLLER_H

#include "../external_libraries/Crow/include/crow.h"
#include "DatabaseManager.h"  
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>

class RouteController {
public:
    RouteController(const std::string& dbPath, const std::string& uri);
    ~RouteController();

    void initRoutes(crow::App<>& app);

private:
    void index(crow::response& res);
    void getData(crow::response& res);

    DatabaseManager databaseManager;  // DatabaseManager instance (fixed variable name)
    mongocxx::client mongoClient;      // MongoDB client instance
};

#endif
