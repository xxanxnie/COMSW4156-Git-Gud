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
  virtual std::string addShelter(std::string request_body);
  virtual std::string deleteShelter(std::string id);
  virtual std::string searchShelterAll();
  virtual std::string updateShelter(std::string id, std::string ORG,
                                    std::string User, std::string location,
                                    int capacity, int curUse);
  std::vector<std::pair<std::string, std::string>> createDBContent(
      std::string ORG, std::string User, std::string location,
      std::string capacity, std::string curUse);
  std::string printShelters(
      std::vector<bsoncxx::document::value>& shelters) const;
  std::string getShelterID(bsoncxx::document::value& shelter);
  std::string collection_name;
  std::unordered_map<std::string, std::string> format;

 private:
  DatabaseManager& dbManager;
};

#endif
