// Copyright 2024 COMSW4156-Git-Gud

#include "Outreach.h"

/**
 * @brief Add the outreach program information to the database.
 * 
 * This method creates a database entry for the outreach program 
 * with the provided details.
 * 
 * @param targetAudience The intended audience for the outreach program.
 * @param programName The name of the outreach program.
 * @param description A brief description of the outreach program.
 * @param programDate The duration of the outreach program.
 * @param location The location where the outreach program will take place.
 * @param contactInfo Contact information for the outreach program.
 * @return A string indicating the success or failure of the operation.
 */
std::string Outreach::addOutreachService(const std::string& targetAudience, 
                                   const std::string& programName,
                                   const std::string& description, 
                                   const std::string& programDate,
                                   const std::string& location,
                                   const std::string& contactInfo) {
    try {
        auto content = createDBContent(targetAudience, programName, description, 
                                        programDate, location, contactInfo);
        dbManager.insertResource(collection_name, content); // Use dbManager

    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return "Error";
    }
    return "Success";
}

/**
 * @brief Create content for the database insertion.
 * 
 * This method formats the provided outreach program details 
 * into a vector of key-value pairs suitable for database insertion.
 * 
 * @param targetAudience The intended audience for the outreach program.
 * @param programName The name of the outreach program.
 * @param description A brief description of the outreach program.
 * @param programDate The duration of the outreach program.
 * @param location The location of the outreach program.
 * @param contactInfo Contact information for the outreach program.
 * @return A vector of key-value pairs representing the outreach program content.
 */
std::vector<std::pair<std::string, std::string>> Outreach::createDBContent(
    const std::string& targetAudience, const std::string& programName,
    const std::string& description, const std::string& programDate,
    const std::string& location, const std::string& contactInfo) {
    
    std::vector<std::pair<std::string, std::string>> content;
    content.push_back({"targetAudience", targetAudience});
    content.push_back({"programName", programName});
    content.push_back({"description", description});
    content.push_back({"programDate", programDate});
    content.push_back({"location", location});
    content.push_back({"contactInfo", contactInfo});
    return content;
}

/**
 * @brief Retrieve all outreach services from the database.
 * 
 * This method queries the database for all outreach services 
 * and returns them in a formatted string.
 * 
 * @return A string representation of all outreach services, 
 *         or "[]" if no services are found.
 */
std::string Outreach::getAllOutreachServices() {
    std::vector<bsoncxx::document::value> result;
    dbManager.findCollection(collection_name, {}, result);  
    if (result.empty()) {
        return "[]";
    }
    return printOutreachServices(result);
}

/**
 * @brief Print the outreach services in a formatted string.
 * 
 * This method formats the outreach services into a string 
 * for display or further processing.
 * 
 * @param services A vector of documents representing the outreach services.
 * @return A formatted string of outreach services.
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
