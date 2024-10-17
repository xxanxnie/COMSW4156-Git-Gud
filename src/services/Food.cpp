#include "Food.h"
#include <iostream>

Food::Food( DatabaseManager& db)
    :  db(db) {
}

void Food::insertFood(const std::vector<std::pair<std::string, std::string>>& resource) {
    try {
        db.insertResource("Food", resource);
        std::cout << "Food resource inserted successfully." << std::endl;  
    } catch (const std::exception& e) {
        std::cerr << "Error inserting food resource: " << e.what() << std::endl;
        throw;  
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

