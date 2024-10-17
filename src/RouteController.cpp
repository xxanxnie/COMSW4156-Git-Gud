// C++ System Header
#include <map>
#include <string>
#include <exception>
#include <iostream>
#include <bsoncxx/json.hpp>

#include "RouteController.h"
#include "Food.h"
// Utility function to handle exceptions
crow::response handleException(const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return crow::response{500, "An error has occurred: " + std::string(e.what())};
}

/**
 * Redirects to the homepage.
 *
 * @return A string containing the name of the html file to be loaded.
 */
void RouteController::index(crow::response& res) {
    res.write("Welcome to the GitGud API. Use the appropriate endpoints to access resources.");
    res.end();
}

// Get resources route
void RouteController::getResources(const crow::request& req, crow::response& res) {
    try {
        const char* resourceType = req.url_params.get("type");
        if (!resourceType) {
            res.code = 400;
            res.write("Error: Resource type is required.");
            res.end();
            return;
        }

        // Implement this method in DatabaseManager
        auto resources = dbManager.getResources(resourceType);
        res.code = 200;
        res.write(bsoncxx::to_json(resources));
        res.end();
    } catch (const std::exception& e) {
        res = handleException(e);
    }
}

// Add resource route
void RouteController::addResource(const crow::request& req, crow::response& res) {
    try {
        // Remove or implement validateInputs
        // if (!validateInputs(req, &res)) {
        //     return;
        // }

        auto resource = bsoncxx::from_json(req.body);
        // Convert bsoncxx::document::value to vector of pairs
        std::vector<std::pair<std::string, std::string>> keyValues;
        for (auto element : resource.view()) {
            keyValues.emplace_back(element.key().to_string(), element.get_utf8().value.to_string());
        }
        dbManager.insertResource("Resources", keyValues);

        res.code = 201; // Created
        res.write("Resource added successfully.");
        res.end();
    } catch (const std::exception& e) {
        res = handleException(e);
    }
}

// Update resource route
void RouteController::updateResource(const crow::request& req, crow::response& res) {
    try {
        // Remove or implement validateInputs
        // if (!validateInputs(req, &res)) {
        //     return;
        // }

        auto resource = bsoncxx::from_json(req.body);
        if (!resource["id"]) {
            res.code = 400;
            res.write("Error: Resource ID is required.");
            res.end();
            return;
        }

        auto id = resource["id"].get_utf8().value.to_string();
        // Convert bsoncxx::document::value to vector of pairs
        std::vector<std::pair<std::string, std::string>> updates;
        for (auto element : resource.view()) {
            if (element.key().to_string() != "id") {
                updates.emplace_back(element.key().to_string(), element.get_utf8().value.to_string());
            }
        }
        dbManager.updateResource("Resources", id, updates);

        res.code = 200;
        res.write("Resource updated successfully.");
        res.end();
    } catch (const std::exception& e) {
        res = handleException(e);
    }
}

// Delete resource route
void RouteController::deleteResource(const crow::request& req, crow::response& res) {
    try {
        const char* resourceId = req.url_params.get("id");
        if (!resourceId) {
            res.code = 400;
            res.write("Error: Resource ID is required.");
            res.end();
            return;
        }

        dbManager.deleteResource("Resources", resourceId);
        res.code = 200;
        res.write("Resource deleted successfully.");
        res.end();
    } catch (const std::exception& e) {
        res = handleException(e);
    }
}

// Add food route
void RouteController::addFood(const crow::request& req, crow::response& res) {
    try {
        // Parse the request body into a BSON document
        auto resource = bsoncxx::from_json(req.body);

        // Convert bsoncxx::document::value to vector of pairs
        std::vector<std::pair<std::string, std::string>> keyValues;
        for (auto element : resource.view()) {
            keyValues.emplace_back(element.key().to_string(), element.get_utf8().value.to_string());
        }

        Food food(dbManager);

        food.insertFood(keyValues);

        res.code = 201; // Created
        res.write("Food resource added successfully.");
        res.end();
    } catch (const std::exception& e) {
        res = handleException(e);
    }
}

// Get all food resources route
void RouteController::getAllFood(const crow::request& req, crow::response& res) {
    try {
        // Instantiate the Food class and pass the dbManager
        Food food(dbManager);

        // Fetch all food resources from the database
        std::string response = food.getAllFood();

        // Send the response
        res.code = 200;
        res.write(response);
        res.end();
    } catch (const std::exception& e) {
        res = handleException(e);
    }
}


// Initialize API Routes
void RouteController::initRoutes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/")
        .methods(crow::HTTPMethod::GET)([this](const crow::request& req, crow::response& res) {
            index(res);
        });

    CROW_ROUTE(app, "/resources")
        .methods(crow::HTTPMethod::GET)([this](const crow::request& req, crow::response& res) {
            getResources(req, res);
        });

    CROW_ROUTE(app, "/resources/add")
        .methods(crow::HTTPMethod::POST)([this](const crow::request& req, crow::response& res) {
            addResource(req, res);
        });

    CROW_ROUTE(app, "/resources/update")
        .methods(crow::HTTPMethod::PATCH)([this](const crow::request& req, crow::response& res) {
            updateResource(req, res);
        });

    CROW_ROUTE(app, "/resources/delete")
        .methods(crow::HTTPMethod::DELETE)([this](const crow::request& req, crow::response& res) {
            deleteResource(req, res);
        });
    
    CROW_ROUTE(app, "/resources/addFood")
    .methods(crow::HTTPMethod::POST)([this](const crow::request& req, crow::response& res) {
        addFood(req, res);
    });

        // New route to get all food resources
    CROW_ROUTE(app, "/resources/getAllFood")
        .methods(crow::HTTPMethod::GET)([this](const crow::request& req, crow::response& res) {
            getAllFood(req, res);
    });
}
