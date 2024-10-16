// Copyright 2024 Wilson, Liang
#ifndef SHELTER_H
#define SHELTER_H
#include <string>
#include <utility>
#include <vector>

#include "DatabaseManager.h"

class Shelter {
 public:
  Shelter(DatabaseManager& dbManager)
      : dbManager(dbManager), collection_name("Shelter") {}

  std::string addShelter(std::string ORG, std::string User,
                         std::string location, int capacity, int curUse);
  std::string deleteShelter();
  std::string searchShelterAll();
  std::string updateShelter();
  std::vector<std::pair<std::string, std::string>> createDBContent(
      std::string ORG, std::string User, std::string location,
      std::string capacity, std::string curUse);
  std::string printShelters(std::vector<bsoncxx::document::view>& shelters) const;
  std::string getShelterID(bsoncxx::document::view& shelter);

 private:
  DatabaseManager& dbManager;
  std::string collection_name;
};

#endif
