// #ifndef FOOD_H
// #define FOOD_H

// #include <string>

// class Food {
// public:
//     Food(const std::string& type, const std::string& quantity);
    
//     std::string getType() const;
//     std::string getQuantity() const;

// private:
//     std::string type;
//     std::string quantity;
// };

// #endif 
#ifndef FOOD_RESOURCE_H
#define FOOD_RESOURCE_H

#include <vector>
#include <string>
#include <utility> // for std::pair
#include "DatabaseManager.h"  // Ensure dbManager is included

class Food {
private:
    std::vector<std::pair<std::string, std::string>> resource;  // Data for the food resource
    DatabaseManager& db;  // Reference to the database manager

public:
    // Constructor to initialize Food with keyValues and dbManager
    Food(const std::vector<std::pair<std::string, std::string>>& reasource, DatabaseManager& db);

    // Method to insert food into the database
    void insertFood();
};

#endif // FOOD_RESOURCE_H
