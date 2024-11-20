// Copyright 2024 COMSW4156-Git-Gud

#include "Healthcare.h"

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/oid.hpp>
#include <iostream>
#include <unordered_set>

#include "DatabaseManager.h"
/*
Name:	provider
City
Address
Description: serviceType
ContactInfo
eligibilityCriteria
Hours of Operation
*/
Healthcare::Healthcare(DatabaseManager& dbManager,
                       const std::string& collection_name)
    : dbManager(dbManager), collection_name(collection_name) {
  cols = std::vector<std::string>({"Name", "City", "Address", "Description",
                                   "ContactInfo", "HoursOfOperation",
                                   "eligibilityCriteria"});
  cleanCache();
}
void Healthcare::cleanCache() {
  for (auto name : cols) {
    format[name] = "";
  }
}
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
 * This function adds a healthcare service to the database by constructing
 * a set of key-value pairs and inserting them into the specified collection.
 *
 * @param provider The name of the healthcare provider.
 * @param serviceType The type of service being provided (e.g., General Care).
 * @param location The location where the service is provided.
 * @param operatingHours The operating hours for the healthcare service.
 * @param eligibilityCriteria The criteria required for eligibility.
 * @param contactInfo The contact information for the healthcare service.
 * @return A string indicating whether the operation was successful (item ID)
 *         or an error message in case of failure.
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
 * @brief Creates a key-value representation of a healthcare service.
 *
 * This function converts the parameters of a healthcare service into a
 * vector of key-value pairs for insertion into the database.
 *
 * @param provider The name of the healthcare provider.
 * @param serviceType The type of service being provided.
 * @param location The location where the service is provided.
 * @param operatingHours The operating hours for the healthcare service.
 * @param eligibilityCriteria The criteria required for eligibility.
 * @param contactInfo The contact information for the healthcare service.
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
 * This function fetches all the documents in the healthcare services
 * collection from the database and returns them in a formatted string.
 *
 * @return A string containing all healthcare services in JSON format.
 *         Returns an empty array ("[]") if no services are found.
 */
std::string Healthcare::getAllHealthcareServices() {
  std::vector<bsoncxx::document::value> result;
  dbManager.findCollection(collection_name, {}, result);
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

std::string Healthcare::deleteHealthcare(std::string id) {
  if (dbManager.deleteResource(collection_name, id)) {
    return "Healthcare record deleted successfully.";
  }

  throw std::runtime_error(
      "Deletion failed: No healthcare record found matching the provided ID.");
}

/**
 * @brief Prints all healthcare services in a formatted string.
 *
 * This function takes a vector of BSON documents representing healthcare
 * services and converts them into a human-readable string, omitting the
 * document's ObjectId.
 *
 * @param services A vector of BSON documents representing healthcare services.
 * @return A string containing the formatted information of all healthcare
 * services.
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

std::string Healthcare::validateHealthcareServiceInput(
    const std::map<std::string, std::string>& content) {
  std::string missingFields;

  std::unordered_set<std::string> requiredFields = {
      "provider", "serviceType", "location", "operatingHours", "contactInfo"};
  std::unordered_set<std::string> allowedFields = {
      "provider",       "serviceType", "location",
      "operatingHours", "contactInfo", "eligibilityCriteria"};

  for (const auto& field : requiredFields) {
    if (content.find(field) == content.end()) {
      missingFields += "Missing " + field + ". ";
    }
  }

  for (const auto& [key, value] : content) {
    if (allowedFields.find(key) == allowedFields.end()) {
      missingFields += "Unexpected field: " + key + ". ";
    }
  }

  return missingFields.empty() ? ""
                               : "Input validation failed: " + missingFields;
}
