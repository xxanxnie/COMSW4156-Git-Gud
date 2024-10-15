// Copyright 2024 Wilson, Liang
#include "Shelter.h"

/**
 * Add the shelter information to our database
 *
 * @param ORG     The organization who gave the infomation.
 * @param target     The target this service provide.
 * @param location     The location of the shelter.
 * @param capacity     The maximum number of users that the shelter can handle.
 * @param curUse       The current users that using this shelter.
 * @return code
 */
std::string Shelter::addShelter(std::string ORG, std::string target,
                                std::string location, int capacity,
                                int curUse) {
  auto content = createDBContent(
      ORG, target, location, std::to_string(capacity), std::to_string(curUse));
  dbManager.insertResource(collection_name, content);
  return "Success";
}
std::vector<std::pair<std::string, std::string>> Shelter::createDBContent(
    std::string ORG, std::string target, std::string location,
    std::string capacity, std::string curUse) {
  std::vector<std::pair<std::string, std::string>> content;
  content.push_back({"ORG", ORG});
  content.push_back({"target", target});
  content.push_back({"location", location});
  content.push_back({"capacity", capacity});
  content.push_back({"curUse", curUse});
  return content;
}
std::string Shelter::updateShelter() { return "Update"; }
std::string Shelter::deleteShelter() { return "Delete"; }
std::string Shelter::searchShelter() const {
  dbManager.printCollection(collection_name);
  return "Search";
}
