// Copyright 2024 COMSW4156-Git-Gud

#include "Counseling.h"

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

#include "DatabaseManager.h"
/*
Name
counselorName
City
Address
Description: specialty
ContactInfo
Hours of Operation
*/
/**
 * @brief Constructs a Counseling object.
 * @param dbManager Reference to the DatabaseManager object.
 */
Counseling::Counseling(DatabaseManager &dbManager)
    : dbManager(dbManager), collection_name("Counseling") {
  cols = std::vector<std::string>({"Name", "counselorName", "City", "Address",
                                   "Description", "ContactInfo",
                                   "HoursOfOperation"});
  cleanCache();
}
void Counseling::cleanCache() {
  for (auto name : cols) {
    format[name] = "";
  }
}
std::string Counseling::checkInputFormat(std::string content) {
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
          "Counseling: The request with unrelative argument.");
    }
  }
  for (auto property : format) {
    if (property.second == "") {
      cleanCache();
      throw std::invalid_argument(
          "Counseling: The request missing some properties.");
    }
  }
  return id;
}
/**
 * @brief Adds a new counselor to the database.
 * @param counselorName The name of the counselor.
 * @param specialty The specialty of the counselor.
 * @return A string indicating ID or an error message.
 */
std::string Counseling::addCounselor(std::string request_body) {
  try {
    cleanCache();
    checkInputFormat(request_body);
    auto content_new = createDBContent();
    std::string ID = dbManager.insertResource(collection_name, content_new);
    return ID;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return "Error: " + std::string(e.what());
  }
}

/**
 * @brief Creates the database content for a counselor.
 * @param counselorName The name of the counselor.
 * @param specialty The specialty of the counselor.
 * @return A vector of key-value pairs representing the counselor's data.
 */
std::vector<std::pair<std::string, std::string>> Counseling::createDBContent() {
  std::vector<std::pair<std::string, std::string>> content;
  for (auto property : format) {
    content.push_back(property);
  }
  return content;
}

/**
 * @brief Deletes a counselor from the database.
 * @param counselorId The ID of the counselor to delete.
 * @return A string indicating the result of the operation.
 * @todo Implement counselor deletion logic.
 */
std::string Counseling::deleteCounselor(const std::string &counselorId) {
  if (dbManager.deleteResource(collection_name, counselorId)) {
    return "Success";
  }
  throw std::runtime_error(
      "Counseling: Document with the specified _id not found.");
}

/**
 * @brief Searches for all counselors in the database.
 * @return A JSON string containing all counselors' information.
 */
std::string Counseling::searchCounselorsAll(int start) {
  std::vector<bsoncxx::document::value> result;
  dbManager.findCollection(start, collection_name, {}, result);
  std::string ret;
  if (!result.empty()) {
    ret = printCounselors(result);
  } else {
    ret = "[]";
  }
  return ret;
}

/**
 * @brief Updates a counselor's information in the database.
 * @param counselorId The ID of the counselor to update.
 * @param counselorName The new name of the counselor.
 * @param specialty The new specialty of the counselor.
 * @return A string indicating the result of the operation.
 * @todo Implement counselor update logic.
 */
std::string Counseling::updateCounselor(std::string request_body) {
  try {
    cleanCache();
    std::string id = checkInputFormat(request_body);
    auto content_new = createDBContent();
    dbManager.updateResource(collection_name, id, content_new);
  } catch (const std::exception &e) {
    return "Error: " + std::string(e.what());
  }
  return "Success";
}

/**
 * @brief Retrieves the ID of a counselor from a BSON document.
 * @param counselor The BSON document containing the counselor's information.
 * @return The ID of the counselor as a string.
 */
// std::string Counseling::getCounselorID(
//     const bsoncxx::document::view &counselor) {
//   std::string id = counselor["_id"].get_oid().value.to_string();
//   std::cout << id << std::endl;
//   return id;
// }

/**
 * @brief Converts a vector of BSON documents to a JSON string.
 * @param counselors A vector of BSON documents representing counselors.
 * @return A JSON string containing all counselors' information.
 */
std::string Counseling::printCounselors(
    std::vector<bsoncxx::document::value> &counselors) const {
  std::string ret = "[";
  for (const auto &counselor : counselors) {
    try {
      ret += bsoncxx::to_json(counselor.view()) + ",";
    } catch (const std::exception &e) {
      std::cerr << "Error processing counselor document: " << e.what()
                << std::endl;
      ret += "{\"error\":\"Unable to process this counselor data\"},";
    }
  }

  if (ret.back() == ',') {
    ret.pop_back();
  }
  ret += "]";
  return ret;
}
