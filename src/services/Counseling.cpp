// Copyright 2024 COMSW4156-Git-Gud

#include "Counseling.h"

#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include "DatabaseManager.h"

/**
 * @brief Constructs a Counseling object.
 * @param dbManager Reference to the DatabaseManager object.
 */
Counseling::Counseling(DatabaseManager &dbManager)
    : dbManager(dbManager), collection_name("Counseling") {}

/**
 * @brief Adds a new counselor to the database.
 * @param counselorName The name of the counselor.
 * @param specialty The specialty of the counselor.
 * @return A string indicating success or an error message.
 */
std::string Counseling::addCounselor(const std::string &counselorName, const std::string &specialty) {
  auto content = createDBContent(counselorName, specialty);
  try {
    dbManager.insertResource(collection_name, content);
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return "Error: " + std::string(e.what());
  }
  return "Success";
}

/**
 * @brief Creates the database content for a counselor.
 * @param counselorName The name of the counselor.
 * @param specialty The specialty of the counselor.
 * @return A vector of key-value pairs representing the counselor's data.
 */
std::vector<std::pair<std::string, std::string>> Counseling::createDBContent(
    const std::string &counselorName, const std::string &specialty) {
  std::vector<std::pair<std::string, std::string>> content;
  content.push_back({"counselorName", counselorName});
  content.push_back({"specialty", specialty});
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
  throw std::runtime_error("Document with the specified _id not found.");
}

/**
 * @brief Searches for all counselors in the database.
 * @return A JSON string containing all counselors' information.
 */
std::string Counseling::searchCounselorsAll() {
  std::vector<bsoncxx::document::value> result;
  dbManager.findCollection(collection_name, {}, result);
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
std::string Counseling::updateCounselor(const std::string &counselorId, const std::string &counselorName, const std::string &specialty) {
  try {
    auto content = createDBContent(counselorName, specialty);
    dbManager.updateResource(collection_name, counselorId, content);
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
std::string Counseling::getCounselorID(const bsoncxx::document::view &counselor) {
  std::string id = counselor["_id"].get_oid().value.to_string();
  std::cout << id << std::endl;
  return id;
}

/**
 * @brief Converts a vector of BSON documents to a JSON string.
 * @param counselors A vector of BSON documents representing counselors.
 * @return A JSON string containing all counselors' information.
 */
std::string Counseling::printCounselors(std::vector<bsoncxx::document::value> &counselors) const {
  std::string ret = "[";
  for (const auto &counselor : counselors) {
    try {
      ret += bsoncxx::to_json(counselor.view()) + ",";
    } catch (const std::exception &e) {
      std::cerr << "Error processing counselor document: " << e.what() << std::endl;
      ret += "{\"error\":\"Unable to process this counselor data\"},";
    }
  }

  if (ret.back() == ',') {
    ret.pop_back();
  }
  ret += "]";
  return ret;
}
