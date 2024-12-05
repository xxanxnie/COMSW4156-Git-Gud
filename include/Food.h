#ifndef FOOD_RESOURCE_H
#define FOOD_RESOURCE_H

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "DatabaseManager.h"

class Food {
 private:
  DatabaseManager& db;
  std::string collection_name;
  std::vector<std::string> cols;

 public:
  Food(DatabaseManager& db, const std::string& collection_name);
  void cleanCache();
  std::string checkInputFormat(std::string content, std::string request_auth);
  std::unordered_map<std::string, std::string> format;
  virtual std::string addFood(std::string request_body, std::string request_auth);
  std::vector<std::pair<std::string, std::string>> createDBContent();
  virtual std::string getAllFood(int start = 0);

  virtual std::string updateFood(std::string request_body, std::string request_auth);

  virtual std::string deleteFood(const std::string& id, std::string request_auth);
};

#endif
