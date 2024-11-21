// Copyright 2024 Wilson, Liang
#ifndef SHELTER_H
#define SHELTER_H
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "DatabaseManager.h"

class Shelter {
 public:
  Shelter(DatabaseManager& dbManager, std::string collection_name);
  void cleanCache();
  std::string checkInputFormat(std::string content);
  virtual std::string addShelter(std::string request_body);
  virtual std::string deleteShelter(std::string id);
  virtual std::string searchShelterAll();
  virtual std::string updateShelter(std::string request_body);
  std::vector<std::pair<std::string, std::string>> createDBContent();
  std::string printShelters(
      std::vector<bsoncxx::document::value>& shelters) const;
  // std::string getShelterID(bsoncxx::document::value& shelter);
  std::string collection_name;
  std::unordered_map<std::string, std::string> format;

 private:
  DatabaseManager& dbManager;
  std::vector<std::string> cols;
};

#endif
