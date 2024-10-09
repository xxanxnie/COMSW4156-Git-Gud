#include "RouteController.h"

// Constructor
RouteController::RouteController(const std::string& uri)
    : mongoClient(mongocxx::uri{uri}) {
    initializeDatabase(); // Call to initialize the database
}

// Initialize database
void RouteController::initializeDatabase() {  // Definition
    try {
        auto db = mongoClient["your_database_name"]; // Replace with your database name

        // Create a collection if it doesn't exist
        db.create_collection("your_collection_name"); // Replace with your collection name

        // Optionally insert initial data
        bsoncxx::builder::stream::document document{};
        document << "key" << "value"; // Adjust this as necessary for your data

        // Insert initial document
        auto result = db["your_collection_name"].insert_one(document.view());
        if (result) {
            std::cout << "Inserted document with id: " 
                      << result->inserted_id().get_oid().value.to_string() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error initializing database: " << e.what() << std::endl;
    }
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
        auto collection = mongoClient["your_database_name"]["your_collection_name"];
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
