// Copyright 2024 COMSW4156-Git-Gud

#include "Healthcare.h"

#include <iostream>
#include <unordered_set>

#include "DatabaseManager.h"

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/oid.hpp>

/*
Name:	provider
City
Address
Description: serviceType
ContactInfo
eligibilityCriteria
Hours of Operation
*/

/**
 * @brief Constructs a Healthcare object.
 * 
 * @param dbManager A reference to the DatabaseManager object for database operations.
 * @param collection_name The name of the collection where healthcare services are stored.
 */
Healthcare::Healthcare(DatabaseManager& dbManager,
                       const std::string& collection_name)
    : dbManager(dbManager), collection_name(collection_name) {
  cols = std::vector<std::string>({"Name", "City", "Address", "Description",
                                   "ContactInfo", "HoursOfOperation",
                                   "eligibilityCriteria"});
  cleanCache();
}
/**
 * @brief Clears the internal cache by resetting all properties to empty strings.
 */
void Healthcare::cleanCache() {
  for (auto name : cols) {
    format[name] = "";
  }
}
/**
 * @brief Validates and parses the input JSON string for a healthcare service.
 * 
 * Checks the input JSON string to ensure all required fields are present and valid. 
 * Extracts the ID if provided.
 * 
 * @param content A JSON string containing the healthcare service data.
 * 
 * @return The extracted ID as a string, or an empty string if no ID is provided.
 * 
 * @throws std::invalid_argument If required fields are missing or unexpected fields are present.
 */
std::string Healthcare::checkInputFormat(std::string content) {
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
          "Healthcare: The request with unrelative argument.");
    }
  }

  for (auto property : format) {
    if (property.second == "") {
      cleanCache();
      throw std::invalid_argument(
          "Healthcare: The request missing some properties.");
    }
  }
  return id;
}
/**
 * @brief Adds a new healthcare service to the database.
 * 
 * @param request_body A JSON string containing the healthcare service data.
 * 
 * @return The ID of the newly added healthcare service, or an error message if the operation fails.
 */
std::string Healthcare::addHealthcareService(std::string request_body) {
  try {
    cleanCache();
    checkInputFormat(request_body);
    auto content_new = createDBContent();
    std::string ID = dbManager.insertResource(collection_name, content_new);
    return ID;
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return "Error: " + std::string(e.what());
  }
}

/**
 * @brief Converts the healthcare service data into key-value pairs for database storage.
 * 
 * @return A vector of key-value pairs representing the healthcare service.
 */
std::vector<std::pair<std::string, std::string>> Healthcare::createDBContent() {
  std::vector<std::pair<std::string, std::string>> content;
  for (auto property : format) {
    content.push_back(property);
  }
  return content;
}

/**
 * @brief Retrieves all healthcare services from the database.
 * 
 * Queries the database for all documents in the healthcare services collection and returns them in JSON format.
 * 
 * @param start The starting index for pagination.
 * 
 * @return A JSON string containing all healthcare services, or an empty array ("[]") if none are found.
 */
std::string Healthcare::getAllHealthcareServices(int start) {
  std::vector<bsoncxx::document::value> result;
  dbManager.findCollection(start, collection_name, {}, result);
  if (result.size() > 0) {
    bsoncxx::builder::basic::array arrayBuilder;
    for (const auto& doc : result) {
      arrayBuilder.append(doc.view());
    }
    std::cout << printHealthcareServices(result);
    return bsoncxx::to_json(arrayBuilder.view());
  }
  return "[]";
}
/**
 * @brief Updates an existing healthcare service in the database.
 * 
 * @param request_body A JSON string containing the updated healthcare service data.
 * 
 * @return "Update" if the operation is successful, or an error message if it fails.
 */
std::string Healthcare::updateHealthcare(std::string request_body) {
  try {
    cleanCache();
    std::string id = checkInputFormat(request_body);
    auto content_new = createDBContent();
    dbManager.updateResource(collection_name, id, content_new);
  } catch (const std::exception& e) {
    return "Error: " + std::string(e.what());
  }
  return "Update";
}
/**
 * @brief Deletes a healthcare service from the database.
 * 
 * @param id The ID of the healthcare service to delete.
 * 
 * @return A success message if the service is deleted, or an error if the operation fails.
 * 
 * @throws std::runtime_error If the specified healthcare record is not found.
 */
std::string Healthcare::deleteHealthcare(std::string id) {
  if (dbManager.deleteResource(collection_name, id)) {
    return "Healthcare record deleted successfully.";
  }

  throw std::runtime_error(
      "Deletion failed: No healthcare record found matching the provided ID.");
}

/**
 * @brief Converts a vector of healthcare services into a human-readable string.
 * 
 * @param services A vector of BSON documents representing healthcare services.
 * 
 * @return A formatted string containing the details of all healthcare services.
 */
std::string Healthcare::printHealthcareServices(
    std::vector<bsoncxx::document::value>& services) const {
  std::string ret;
  for (auto hs : services) {
    for (auto element : hs.view()) {
      if (element.type() != bsoncxx::type::k_oid) {
        ret += element.get_string().value.to_string() + "\n";
      }
    }
    ret += "\n";
  }
  return ret;
}

// std::string Healthcare::validateHealthcareServiceInput(
//     const std::map<std::string, std::string>& content) {
//   std::string missingFields;

//   std::unordered_set<std::string> requiredFields = {
//       "provider", "serviceType", "location", "operatingHours",
//       "contactInfo"};
//   std::unordered_set<std::string> allowedFields = {
//       "provider",       "serviceType", "location",
//       "operatingHours", "contactInfo", "eligibilityCriteria"};

//   for (const auto& field : requiredFields) {
//     if (content.find(field) == content.end()) {
//       missingFields += "Missing " + field + ". ";
//     }
//   }

//   for (const auto& [key, value] : content) {
//     if (allowedFields.find(key) == allowedFields.end()) {
//       missingFields += "Unexpected field: " + key + ". ";
//     }
//   }

//   return missingFields.empty() ? ""
//                                : "Input validation failed: " + missingFields;
// }
