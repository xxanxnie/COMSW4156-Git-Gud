#include "Food.h"
#include <iostream>

Food::Food(const std::vector<std::pair<std::string, std::string>>& resource, DatabaseManager& db)
    : resource(resource), db(db) {
}

void Food::insertFood() {
    try {
        db.insertResource("Food", resource);
        std::cout << "Food resource inserted successfully." << std::endl;  
    } catch (const std::exception& e) {
        std::cerr << "Error inserting food resource: " << e.what() << std::endl;
        throw;  
    }
}


