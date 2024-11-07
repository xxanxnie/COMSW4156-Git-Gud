// Copyright 2024 Wilson, Liang
#ifndef SHELTER_H
#define SHELTER_H
#include <string>
#include <utility>
#include <vector>

#include "DatabaseManager.h"

class Shelter {
 public:
  Shelter(DatabaseManager& dbManager, std::string collection_name)
      : dbManager(dbManager), collection_name(collection_name) {}

  virtual std::string addShelter(std::string ORG, std::string User,
                                 std::string location, int capacity,
                                 int curUse);
  virtual std::string deleteShelter(std::string id);
  virtual std::string searchShelterAll();
  virtual std::string updateShelter();
  std::vector<std::pair<std::string, std::string>> createDBContent(
      std::string ORG, std::string User, std::string location,
      std::string capacity, std::string curUse);
  std::string printShelters(
      std::vector<bsoncxx::document::value>& shelters) const;
  std::string getShelterID(bsoncxx::document::value& shelter);
  std::string collection_name;

 private:
  DatabaseManager& dbManager;
};

#endif
