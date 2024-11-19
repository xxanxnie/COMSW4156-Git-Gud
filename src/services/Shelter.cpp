// Copyright 2024 COMSW4156-Git-Gud
#include "Shelter.h"
/* property in database
Name
City
Address
Description
ContactInfo
HoursOfOperation
Capacity
CurrentUse
*/
Shelter::Shelter(DatabaseManager &dbManager, std::string collection_name)
    : dbManager(dbManager), collection_name(collection_name) {
  std::vector<std::string> cols({"Name", "City", "Address", "Description",
                             "ContactInfo", "HoursOfOperation", "Capacity",
                             "CurrentUse"});
  for (auto name : cols) {
    format[name] = "";
  }
}
/**
 * Add the shelter information to our database
 *
 * @param ORG     The organization who gave the infomation.
 * @param User     The target this service provide for.
 * @param location     The location of the shelter.
 * @param capacity     The maximum number of users that the shelter can handle.
 * @param curUse       The current users that using this shelter.
 * @return item ID in database or Error message
 */
std::string Shelter::addShelter(std::string request_body) {
  try {
    auto resource = bsoncxx::from_json(request_body);
    std::vector<std::string> content;
    for (auto element : resource.view()) {
      if (element.key().to_string() != "id") {
        content.push_back(element.get_utf8().value.to_string());
      }
    }
    int capacity = atoi(content[3].c_str());
    int current = atoi(content[4].c_str());
    if (capacity <= 0 || current > capacity) {
      throw std::invalid_argument("The request with invalid argument.");
    }

    auto content_new = createDBContent(
        content[0], content[1], content[2], content[3], content[4]);
    std::string ID = dbManager.insertResource(collection_name, content_new);
    return ID;
  } catch (const std::exception &e) {
    return "Error: " + std::string(e.what());
  }
}

/**
 * Concat the input into data content format
 *
 * @param ORG     The organization who gave the infomation.
 * @param User     The target this service provide for.
 * @param location     The location of the shelter.
 * @param capacity     The maximum number of users that the shelter can handle.
 * @param curUse       The current users that using this shelter.
 * @return std::vector<std::pair<std::string, std::string>> concat the value in
 * to vector of pair
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

std::string Shelter::updateShelter(std::string id, std::string ORG,
                                   std::string User, std::string location,
                                   int capacity, int curUse) {
  try {
    auto content = createDBContent(
        ORG, User, location, std::to_string(capacity), std::to_string(curUse));
    dbManager.updateResource(collection_name, id, content);
  } catch (const std::exception &e) {
    return "Error: " + std::string(e.what());
  }
  return "Update";
}
std::string Shelter::deleteShelter(std::string id) {
  if (dbManager.deleteResource(collection_name, id)) {
    return "SUC";
  }
  throw std::runtime_error("Document with the specified _id not found.");
}
