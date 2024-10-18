// Copyright 2024 COMSW4156-Git-Gud

#include "Healthcare.h"
#include "DatabaseManager.h"
#include <iostream>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/oid.hpp>

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
 * @return A string indicating whether the operation was successful ("Success")
 *         or an error message in case of failure.
 */
std::string Healthcare::addHealthcareService(const std::string& provider, const std::string& serviceType, 
                                                    const std::string& location, const std::string& operatingHours, 
                                                    const std::string& eligibilityCriteria, const std::string& contactInfo) {
    try {
        auto content = createDBContent(provider, serviceType, location, operatingHours, eligibilityCriteria, contactInfo);
        dbManager.insertResource(collection_name, content);
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return "Error: " + std::string(e.what());
    }
    return "Success";
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
std::vector<std::pair<std::string, std::string>> Healthcare::createDBContent(const std::string& provider, 
                                                                                    const std::string& serviceType, 
                                                                                    const std::string& location, 
                                                                                    const std::string& operatingHours, 
                                                                                    const std::string& eligibilityCriteria, 
                                                                                    const std::string& contactInfo) {
    std::vector<std::pair<std::string, std::string>> content;
    content.push_back({"provider", provider});
    content.push_back({"serviceType", serviceType});
    content.push_back({"location", location});
    content.push_back({"operatingHours", operatingHours});
    content.push_back({"eligibilityCriteria", eligibilityCriteria});
    content.push_back({"contactInfo", contactInfo});
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

/**
 * @brief Prints all healthcare services in a formatted string.
 *
 * This function takes a vector of BSON documents representing healthcare
 * services and converts them into a human-readable string, omitting the
 * document's ObjectId.
 *
 * @param services A vector of BSON documents representing healthcare services.
 * @return A string containing the formatted information of all healthcare services.
 */
std::string Healthcare::printHealthcareServices(
    std::vector<bsoncxx::document::value> &services) const {
  std::string ret;
  for (auto hs : services) {
    for (auto element : hs.view()) {
      if (element.type() != bsoncxx::type::k_oid) {
        ret += element.get_string().value.to_string()+ "\n";
      }
      ret += "\n";
    }
  }
  return ret;
}

