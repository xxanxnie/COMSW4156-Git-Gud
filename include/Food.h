#ifndef FOOD_RESOURCE_H
#define FOOD_RESOURCE_H

#include <vector>
#include <string>
#include <utility> 
#include "DatabaseManager.h"  

class Food {
private:
    std::vector<std::pair<std::string, std::string>> resource;  
    DatabaseManager& db;  // Reference to the database manager

public:
    Food(const std::vector<std::pair<std::string, std::string>>& reasource, DatabaseManager& db);

    void insertFood();
};

#endif 
