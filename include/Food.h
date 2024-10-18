#ifndef FOOD_RESOURCE_H
#define FOOD_RESOURCE_H

#include <string>
#include <utility>
#include <vector>

#include "DatabaseManager.h"

class Food {
 private:
  // std::vector<std::pair<std::string, std::string>> resource;
  DatabaseManager& db;  // Reference to the database manager

 public:
  Food(DatabaseManager& db);

  virtual std::string addFood(
      const std::vector<std::pair<std::string, std::string>>& reasource);

  virtual std::string getAllFood();
};

#endif
