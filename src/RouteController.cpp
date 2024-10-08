#include "RouteController.h"

// Constructor
RouteController::RouteController(const std::string& dbPath, const std::string& uri)
    : databaseManager(dbPath), mongoClient(mongocxx::uri{uri}) {
    DatabaseManager.start(); // Start MongoDB
}

// Destructor
RouteController::~RouteController() {
    DatabaseManager.stop(); // Stop MongoDB when done
}

// Initialize routes
void RouteController::initRoutes(crow::App<>& app) {
    CROW_ROUTE(app, "/")([this](const crow::request& req, crow::response& res) {
        index(res);
    });

    CROW_ROUTE(app, "/data")([this](const crow::request& req, crow::response& res) {
        getData(res); // Fetch data from MongoDB
    });
}

// Index route handler
void RouteController::index(crow::response& res) {
    res.write("Welcome to the service!");
    res.end();
}

// Data fetching route handler
void RouteController::getData(crow::response& res) {
    try {
        auto collection = mongoClient["database_name"]["collection_name"];
        auto cursor = collection.find({}); // Find all documents

        // Create a JSON array to store results
        bsoncxx::builder::stream::array resultArray;
        for (const auto& doc : cursor) {
            resultArray.append(doc);
        }

        // Convert BSON array to JSON
        auto jsonString = bsoncxx::to_json(resultArray.view());

        // Write JSON response
        res.set_header("Content-Type", "application/json");
        res.write(jsonString);
    } catch (const std::exception& e) {
        res.code = 500; // Internal Server Error
        res.write("Error fetching data: " + std::string(e.what()));
    }
    res.end();
}
