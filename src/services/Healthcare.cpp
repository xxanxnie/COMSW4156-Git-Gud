// Copyright 2024 COMSW4156-Git-Gud

#include "Healthcare.h"

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/oid.hpp>
#include <iostream>
#include <unordered_set>

#include "DatabaseManager.h"
/*
Name:	provider
Address	
Category	
Description: serviceType	
ContactInfo
eligibilityCriteria
Hours of Operation
*/
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
std::string Healthcare::addHealthcareService(
    const std::map<std::string, std::string>& updates) {
  try {
    auto content = createDBContent(updates);
    std::string ID = dbManager.insertResource(collection_name, content);
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
std::vector<std::pair<std::string, std::string>> Healthcare::createDBContent(
    const std::map<std::string, std::string>& updates) {
  std::vector<std::pair<std::string, std::string>> content;

  if (updates.count("provider"))
    content.push_back({"provider", updates.at("provider")});
  if (updates.count("serviceType"))
    content.push_back({"serviceType", updates.at("serviceType")});
  if (updates.count("location"))
    content.push_back({"location", updates.at("location")});
  if (updates.count("operatingHours"))
    content.push_back({"operatingHours", updates.at("operatingHours")});
  if (updates.count("eligibilityCriteria"))
    content.push_back(
        {"eligibilityCriteria", updates.at("eligibilityCriteria")});
  if (updates.count("contactInfo"))
    content.push_back({"contactInfo", updates.at("contactInfo")});

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
  if (result.empty()) {
    return "[]";
  }
  return printHealthcareServices(result);
}

std::string Healthcare::updateHealthcare(
    const std::string& id, const std::map<std::string, std::string>& updates) {
  try {
    auto content = createDBContent(updates);
    dbManager.updateResource(collection_name, id, content);
  } catch (const std::exception& e) {
    return "Update failed: " + std::string(e.what()) +
           ". Please check the input and try again.";
  }

  return "Healthcare record updated successfully.";
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
