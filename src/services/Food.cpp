// Copyright 2024 COMSW4156-Git-Gud

#include "Food.h"
#include <iostream>

/**
 * @brief Constructs a `Food` object with a reference to the database manager.
 * 
 * @param db A reference to the `DatabaseManager` instance used for database operations.
*/
Food::Food( DatabaseManager& db)
    :  db(db) {
}

/**
 * @brief Adds a food resource to the database.
 * 
 * This method takes a vector of key-value pairs representing the food resource and 
 * stores it in the database. The expected key-value pairs for the food resource 
 * include:
 * - FoodType: Type of food (e.g., "Fruits")
 * - Provider: The provider of the food (e.g., "LocalFarm")
 * - location: The location of the provider (e.g., "Brooklyn")
 * - quantity: The amount of food available (e.g., "100")
 * - expirationDate: The date the food expires (e.g., "2024-12-31")
 * 
 * @param resource A vector of key-value pairs representing the food resource to be added.
 * 
 * @exception std::exception Throws if an error occurs during the database insertion.
*/
void Food::addFood(const std::vector<std::pair<std::string, std::string>>& resource) {
    try {
        db.insertResource("Food", resource);
        std::cout << "Food resource inserted successfully." << std::endl;  
    } catch (const std::exception& e) {
        std::cerr << "Error inserting food resource: " << e.what() << std::endl;
        throw;  
    }
}

/**
 * @brief Retrieves all food resources from the database.
 * 
 * This method queries the database for all food resources and returns them 
 * as a JSON-formatted string. If no food items are found, it returns an empty JSON array.
 * 
 * @return A string containing all food resources in JSON format.
 * 
 * @exception std::exception Throws if an error occurs during the database query or data serialization.
*/
std::string Food::getAllFood() {
    // Vector to store BSON documents fetched from the collection
    std::vector<bsoncxx::document::value> foodItems;
    
    // Query the database to get all food documents
    db.findCollection("Food", {}, foodItems);
    
    // If no food items are found, return an empty JSON array
    if (foodItems.empty()) {
        return "[]";
    }

    // Use BSON to JSON conversion to return food items as a JSON array
    bsoncxx::builder::basic::array arrayBuilder;
    for (const auto& doc : foodItems) {
        arrayBuilder.append(doc.view());
    }

    return bsoncxx::to_json(arrayBuilder.view());
}
