#ifndef FOOD_RESOURCE_H
#define FOOD_RESOURCE_H

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include "DatabaseManager.h"

class Food {
 private:
  DatabaseManager& db;
  std::vector<std::string> cols;

 public:
  Food(DatabaseManager& db);
  void cleanCache();
  std::string checkInputFormat(std::string content);
  std::unordered_map<std::string, std::string> format;
  virtual std::string addFood(std::string request_body);
  std::vector<std::pair<std::string, std::string>> createDBContent();
  virtual std::string getAllFood();

  virtual std::string updateFood(std::string request_body);

  virtual std::string deleteFood(const std::string& id);
};

#endif
