// Copyright 2024 COMSW4156-Git-Gud

#include "Outreach.h"
/*
Name: programName
City
Address
Description
TargetAudience
ContactInfo
Hours of Operation
*/

/**
 * @brief Constructs an Outreach object.
 *
 * Initializes the Outreach object with a reference to the database manager and
 * the collection name.
 *
 * @param dbManager A reference to the DatabaseManager for database operations.
 * @param collection_name The name of the collection where outreach services are
 * stored.
 */
Outreach::Outreach(DatabaseManager& dbManager,
                   const std::string& collection_name)
    : dbManager(dbManager), collection_name(collection_name) {
  cols = std::vector<std::string>({"Name", "City", "Address", "Description",
                                   "ContactInfo", "HoursOfOperation",
                                   "TargetAudience"});
  cleanCache();
}
/**
 * @brief Clears the internal cache.
 *
 * Resets all properties to empty strings in preparation for new input.
 */
void Outreach::cleanCache() {
  for (auto name : cols) {
    format[name] = "";
  }
}
/**
 * @brief Validates and parses the input JSON string for an outreach service.
 *
 * Ensures all required fields are present and valid. Extracts the ID if
 * provided.
 *
 * @param content A JSON string containing the outreach service data.
 *
 * @return The extracted ID as a string, or an empty string if no ID is
 * provided.
 *
 * @throws std::invalid_argument If required fields are missing or unexpected
 * fields are present.
 */
std::string Outreach::checkInputFormat(std::string content,
                                       std::string authToken) {
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
          "Outreach: The request with unrelative argument.");
    }
  }

  format["authToken"] = authToken;
  for (auto property : format) {
    if (property.second == "") {
      cleanCache();
      throw std::invalid_argument(
          "Outreach: The request missing some properties.");
    }
  }
  return id;
}
/**
 * @brief Adds a new outreach service to the database.
 *
 * @param request_body A JSON string containing the outreach service data.
 *
 * @return The ID of the newly added outreach service, or an error message if
 * the operation fails.
 */
std::string Outreach::addOutreachService(std::string request_body,
                                         std::string request_auth) {
  try {
    cleanCache();
    checkInputFormat(request_body, request_auth);
    auto content_new = createDBContent();
    std::string ID = dbManager.insertResource(collection_name, content_new);
    return ID;
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return "Error: " + std::string(e.what());
  }
}

/**
 * @brief Formats the outreach service data into key-value pairs for database
 * storage.
 *
 * @return A vector of key-value pairs representing the outreach service data.
 */
std::vector<std::pair<std::string, std::string>> Outreach::createDBContent() {
  std::vector<std::pair<std::string, std::string>> content;
  for (auto property : format) {
    content.push_back(property);
  }
  return content;
}

/**
 * @brief Retrieves all outreach services from the database.
 *
 * Queries the database and retrieves all documents in the outreach services
 * collection.
 *
 * @param start The starting index for pagination.
 *
 * @return A JSON string containing all outreach services, or an empty array
 * ("[]") if none are found.
 */
std::string Outreach::getAllOutreachServices(int start) {
  std::vector<bsoncxx::document::value> result;
  dbManager.findCollection(start, collection_name, {}, result);
  if (result.size() > 0) {
    bsoncxx::builder::basic::array arrayBuilder;
    for (const auto& doc : result) {
      arrayBuilder.append(doc.view());
    }
    std::cout << printOutreachServices(result);
    // getShelterID(result[0]);
    return bsoncxx::to_json(arrayBuilder.view());
  }
  return "[]";
}

/**
 * @brief Converts a list of outreach services into a human-readable format.
 *
 * @param services A vector of BSON documents representing the outreach
 * services.
 *
 * @return A formatted string containing the details of all outreach services.
 */
std::string Outreach::printOutreachServices(
    const std::vector<bsoncxx::document::value>& services) const {
  std::string ret;
  for (const auto& hs : services) {
    for (const auto& element : hs.view()) {
      // Ensure type checking is done correctly
      if (element.type() == bsoncxx::type::k_utf8) {
        ret += element.get_utf8().value.to_string() + "\n";  // Fixed extraction
      }
    }
    ret += "\n";
  }
  return ret;
}
/**
 * @brief Deletes an outreach service from the database.
 *
 * @param id The ID of the outreach service to delete.
 *
 * @return A success message if the deletion is successful.
 *
 * @throws std::runtime_error If the specified outreach service is not found.
 */
std::string Outreach::deleteOutreach(std::string id, std::string request_auth) {
  if (dbManager.deleteResource(collection_name, id, request_auth)) {
    return "Outreach Service deleted successfully.";
  }
  throw std::runtime_error("Document with the specified _id not found.");
}
/**
 * @brief Updates an existing outreach service in the database.
 *
 * @param request_body A JSON string containing the updated outreach service
 * data.
 *
 * @return A success message if the operation is successful, or an error message
 * if it fails.
 */
std::string Outreach::updateOutreach(std::string request_body,
                                     std::string request_auth) {
  try {
    cleanCache();
    std::string id = checkInputFormat(request_body, request_auth);
    auto content_new = createDBContent();
    dbManager.updateResource(collection_name, id, content_new);
  } catch (const std::exception& e) {
    // Return error message if there is an exception
    return "Error: " + std::string(e.what());
  }
  // Return success message
  return "Outreach Service updated successfully.";
}
