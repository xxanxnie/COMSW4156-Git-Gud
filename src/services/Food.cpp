// Copyright 2024 COMSW4156-Git-Gud

#include "Food.h"
#include <iostream>

Food::Food( DatabaseManager& db)
    :  db(db) {
}

std::string Food::addFood(const std::vector<std::pair<std::string, std::string>>& resource) {
    try {
        db.insertResource("Food", resource);
        return "Success";  
    } catch (const std::exception& e) {
        std::cerr << "Error inserting food resource: " << e.what() << std::endl;
        return "Error inserting food resource: " + std::string(e.what());
    }
}


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
