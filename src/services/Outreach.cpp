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
Outreach::Outreach(DatabaseManager& dbManager,
                   const std::string& collection_name)
    : dbManager(dbManager), collection_name(collection_name) {
  cols = std::vector<std::string>({"Name", "City", "Address", "Description",
                                   "ContactInfo", "HoursOfOperation",
                                   "TargetAudience"});
  cleanCache();
}
void Outreach::cleanCache() {
  for (auto name : cols) {
    format[name] = "";
  }
}
std::string Outreach::checkInputFormat(std::string content) {
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
 * @return A string indicating the ID or failure of the operation.
 */
std::string Outreach::addOutreachService(std::string request_body) {
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
 * @return A vector of key-value pairs representing the outreach program
 * content.
 */
std::vector<std::pair<std::string, std::string>> Outreach::createDBContent() {
  std::vector<std::pair<std::string, std::string>> content;
  for (auto property : format) {
    content.push_back(property);
  }
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
std::string Outreach::getAllOutreachServices(int start) {
  std::vector<bsoncxx::document::value> result;
  dbManager.findCollection(start, collection_name, {}, result);
  if (result.size() > 0) {
    bsoncxx::builder::basic::array arrayBuilder;
    for (const auto &doc : result) {
      arrayBuilder.append(doc.view());
    }
    std::cout << printOutreachServices(result);
    // getShelterID(result[0]);
    return bsoncxx::to_json(arrayBuilder.view());
  }
  return "[]";
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

std::string Outreach::deleteOutreach(std::string id) {
  if (dbManager.deleteResource(collection_name, id)) {
    return "Outreach Service deleted successfully.";
  }
  throw std::runtime_error("Document with the specified _id not found.");
}

std::string Outreach::updateOutreach(std::string request_body) {
  try {
    cleanCache();
    std::string id = checkInputFormat(request_body);
    auto content_new = createDBContent();
    dbManager.updateResource(collection_name, id, content_new);
  } catch (const std::exception& e) {
    // Return error message if there is an exception
    return "Error: " + std::string(e.what());
  }
  // Return success message
  return "Outreach Service updated successfully.";
}
