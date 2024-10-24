// Copyright 2024 COMSW4156-Git-Gud
#include "Shelter.h"

/**
 * Add the shelter information to our database
 *
 * @param ORG     The organization who gave the infomation.
 * @param User     The target this service provide for.
 * @param location     The location of the shelter.
 * @param capacity     The maximum number of users that the shelter can handle.
 * @param curUse       The current users that using this shelter.
 * @return String Success or Error message
 */
std::string Shelter::addShelter(std::string ORG, std::string User,
                                std::string location, int capacity,
                                int curUse) {
  try {
    auto content = createDBContent(
        ORG, User, location, std::to_string(capacity), std::to_string(curUse));
    dbManager.insertResource(collection_name, content);
  } catch (const std::exception &e) {
    return "Error: " + std::string(e.what());
  }
  return "Success";
}

/**
 * Concat the input into data content format
 *
 * @param ORG     The organization who gave the infomation.
 * @param User     The target this service provide for.
 * @param location     The location of the shelter.
 * @param capacity     The maximum number of users that the shelter can handle.
 * @param curUse       The current users that using this shelter.
 * @return std::vector<std::pair<std::string, std::string>> concat the value in to vector of pair
 */
std::vector<std::pair<std::string, std::string>> Shelter::createDBContent(
    std::string ORG, std::string User, std::string location,
    std::string capacity, std::string curUse) {
  std::vector<std::pair<std::string, std::string>> content;
  content.push_back({"ORG", ORG});
  content.push_back({"User", User});
  content.push_back({"location", location});
  content.push_back({"capacity", capacity});
  content.push_back({"curUse", curUse});
  return content;
}

/**
 * Get the all data content in Shelter collection, and print it
 *
 * @return string, concat the content value 
 */
std::string Shelter::searchShelterAll() {
  std::vector<bsoncxx::document::value> result;
  dbManager.findCollection(collection_name, {}, result);
  std::string ret = "[]";

  if (result.size() > 0) {
    ret = printShelters(result);
    // getShelterID(result[0]);
  }
  return ret;
}

std::string Shelter::getShelterID(bsoncxx::document::value &shelter) {
  std::string id = shelter["_id"].get_oid().value.to_string();
  return id;
}

std::string Shelter::printShelters(
    std::vector<bsoncxx::document::value> &shelters) const {
  std::string ret;
  for (auto shelter : shelters) {
    for (auto element : shelter.view()) {
      if (element.type() != bsoncxx::type::k_oid) {
        ret += element.get_string().value.to_string() + " ";
      }
    }
    ret += "\n";
  }
  return ret;
}

std::string Shelter::updateShelter() { return "Update"; }
std::string Shelter::deleteShelter() { return "Delete"; }
