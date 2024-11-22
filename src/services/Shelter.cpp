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

/**
 * @brief Constructs a Shelter object.
 * 
 * Initializes the Shelter object with a reference to the database manager and the collection name.
 * 
 * @param dbManager A reference to the DatabaseManager for database operations.
 * @param collection_name The name of the collection where shelter data is stored.
 */
Shelter::Shelter(DatabaseManager &dbManager, std::string collection_name)
    : dbManager(dbManager), collection_name(collection_name) {
  cols = std::vector<std::string>({"Name", "City", "Address", "Description",
                                   "ContactInfo", "HoursOfOperation", "ORG",
                                   "TargetUser", "Capacity", "CurrentUse"});
  cleanCache();
}
/**
 * @brief Clears the internal cache.
 * 
 * Resets all property values in the cache to empty strings.
 */
void Shelter::cleanCache() {
  for (auto name : cols) {
    format[name] = "";
  }
}
/**
 * @brief Validates and parses the input JSON string for a shelter entry.
 * 
 * Ensures all required fields are present, validates capacity and current use, 
 * and extracts the ID if provided.
 * 
 * @param content A JSON string containing the shelter data.
 * 
 * @return The extracted ID as a string, or an empty string if no ID is provided.
 * 
 * @throws std::invalid_argument If required fields are missing or contain invalid values.
 */
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
 * @brief Adds a new shelter to the database.
 * 
 * @param request_body A JSON string containing the shelter data.
 * 
 * @return The ID of the newly added shelter, or an error message if the operation fails.
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
 * @brief Formats the shelter data into key-value pairs for database insertion.
 * 
 * @return A vector of key-value pairs representing the shelter data.
 */
std::vector<std::pair<std::string, std::string>> Shelter::createDBContent() {
  std::vector<std::pair<std::string, std::string>> content;
  for (auto property : format) {
    content.push_back(property);
  }
  return content;
}
/**
 * @brief Retrieves all shelters from the database and prints their details.
 * 
 * @param start The starting index for pagination.
 * 
 * @return A JSON string containing all shelters, or an empty array ("[]") if none are found.
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
/**
 * @brief Formats the details of all shelters into a human-readable string.
 * 
 * @param shelters A vector of BSON documents representing shelters.
 * 
 * @return A formatted string of shelter details.
 */
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
/**
 * @brief Updates an existing shelter in the database.
 * 
 * @param request_body A JSON string containing the updated shelter data.
 * 
 * @return A success message if the operation is successful, or an error message if it fails.
 */
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
/**
 * @brief Deletes a shelter from the database.
 * 
 * @param id The ID of the shelter to delete.
 * 
 * @return A success message if the deletion is successful.
 * 
 * @throws std::runtime_error If the specified shelter is not found.
 */
std::string Shelter::deleteShelter(std::string id) {
  if (dbManager.deleteResource(collection_name, id)) {
    return "SUC";
  }
  throw std::runtime_error(
      "Shelter Document with the specified _id not found.");
}
