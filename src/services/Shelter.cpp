// Copyright 2024 COMSW4156-Git-Gud
#include "Shelter.h"
/* property in database
Name
City
Address
Description
ContactInfo
HoursOfOperation
ORG
TargetUser
Capacity
CurrentUse
*/
Shelter::Shelter(DatabaseManager &dbManager, std::string collection_name)
    : dbManager(dbManager), collection_name(collection_name) {
  cols = std::vector<std::string>({"Name", "City", "Address", "Description",
                                   "ContactInfo", "HoursOfOperation", "ORG",
                                   "TargetUser", "Capacity", "CurrentUse"});
  cleanCache();
}
void Shelter::cleanCache() {
  for (auto name : cols) {
    format[name] = "";
  }
}
std::string Shelter::checkInputFormat(std::string content) {
  auto resource = bsoncxx::from_json(content);
  std::string id;
  for (auto element : resource.view()) {
    if (format.find(element.key().to_string()) != format.end()) {
      format[element.key().to_string()] = element.get_utf8().value.to_string();
    } else {
      if (element.key().to_string() == "id") {
        id = element.get_utf8().value.to_string();
        continue;
      }
      cleanCache();
      throw std::invalid_argument(
          "Shelter: The request with unrelative argument.");
    }
  }
  int capacity = atoi(format["Capacity"].c_str());
  int current = atoi(format["CurrentUse"].c_str());
  if (capacity <= 0 || current > capacity) {
    cleanCache();
    throw std::invalid_argument("Shelter: The request with invalid argument.");
  }
  for (auto property : format) {
    if (property.second == "") {
      cleanCache();
      throw std::invalid_argument(
          "Shelter: The request missing some properties.");
    }
  }
  return id;
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
    cleanCache();
    checkInputFormat(request_body);
    auto content_new = createDBContent();
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
std::vector<std::pair<std::string, std::string>> Shelter::createDBContent() {
  std::vector<std::pair<std::string, std::string>> content;
  for (auto property : format) {
    content.push_back(property);
  }
  return content;
}

/**
 * Get the all data content in Shelter collection, and print it
 *
 * @return string, concat the content value
 */
std::string Shelter::searchShelterAll(int start) {
  std::vector<bsoncxx::document::value> result;
  dbManager.findCollection(start, collection_name, {}, result);

  if (result.size() > 0) {
    bsoncxx::builder::basic::array arrayBuilder;
    for (const auto &doc : result) {
      arrayBuilder.append(doc.view());
    }
    std::cout << printShelters(result);
    // getShelterID(result[0]);
    return bsoncxx::to_json(arrayBuilder.view());
  }
  return "[]";
}

// std::string Shelter::getShelterID(bsoncxx::document::value &shelter) {
//   std::string id = shelter["_id"].get_oid().value.to_string();
//   return id;
// }

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

std::string Shelter::updateShelter(std::string request_body) {
  try {
    cleanCache();
    std::string id = checkInputFormat(request_body);
    auto content_new = createDBContent();
    dbManager.updateResource(collection_name, id, content_new);
  } catch (const std::exception &e) {
    return "Error: " + std::string(e.what());
  }
  return "Update";
}
std::string Shelter::deleteShelter(std::string id) {
  if (dbManager.deleteResource(collection_name, id)) {
    return "SUC";
  }
  throw std::runtime_error(
      "Shelter Document with the specified _id not found.");
}
