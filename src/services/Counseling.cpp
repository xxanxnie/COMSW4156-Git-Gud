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

// Constructor: Initialize Counseling with a DatabaseManager and set the collection name
Counseling::Counseling(DatabaseManager &dbManager)
    : dbManager(dbManager), collection_name("Counseling") {}

// Add a new counselor to the database
std::string Counseling::addCounselor(const std::string &counselorName, const std::string &specialty)
{
  // Create content for the database entry
  auto content = createDBContent(counselorName, specialty);
  try
  {
    // Attempt to insert the new counselor into the database
    dbManager.insertResource(collection_name, content);
  }
  catch (const std::exception &e)
  {
    // If an error occurs, log it and return an error message
    std::cerr << e.what() << '\n';
    return "Error: " + std::string(e.what());
  }
  return "Success";
}

// Create a vector of key-value pairs for database content
std::vector<std::pair<std::string, std::string>> Counseling::createDBContent(
    const std::string &counselorName, const std::string &specialty)
{
  std::vector<std::pair<std::string, std::string>> content;
  content.push_back({"counselorName", counselorName});
  content.push_back({"specialty", specialty});
  return content;
}

// Delete a counselor from the database (placeholder implementation)
std::string Counseling::deleteCounselor(const std::string &counselorId)
{
  // TODO(brendon): Implement counselor deletion logic
  return "Delete";
}

// Retrieve all counselors from the database
std::string Counseling::searchCounselorsAll()
{
  std::vector<bsoncxx::document::value> result;
  // Query the database for all counselors
  dbManager.findCollection(collection_name, {}, result);
  std::string ret;
  if (!result.empty())
  {
    // If counselors are found, format them as a string
    ret = printCounselors(result);
  }
  else
  {
    // If no counselors are found, return an empty JSON array
    ret = "[]";
  }
  return ret;
}

// Update a counselor's information (placeholder implementation)
std::string Counseling::updateCounselor(const std::string &counselorId, const std::string &field, const std::string &value)
{
  // TODO(brendon): Implement counselor update logic
  return "Update";
}

// Extract the counselor ID from a BSON document
std::string Counseling::getCounselorID(const bsoncxx::document::view &counselor)
{
  std::string id = counselor["_id"].get_oid().value.to_string();
  std::cout << id << std::endl;
  return id;
}

// Convert a vector of BSON documents to a JSON string
std::string Counseling::printCounselors(std::vector<bsoncxx::document::value> &counselors) const
{
  std::string ret = "[";
  for (const auto &counselor : counselors)
  {
    try
    {
      // Convert each counselor document to JSON and append to the result string
      ret += bsoncxx::to_json(counselor.view()) + ",";
    }
    catch (const std::exception &e)
    {
      // If an error occurs while processing a counselor, log it and add an error JSON object
      std::cerr << "Error processing counselor document: " << e.what() << std::endl;
      ret += "{\"error\":\"Unable to process this counselor data\"},";
    }
  }

  // Remove the trailing comma if there are any counselors
  if (ret.back() == ',')
  {
    ret.pop_back();
  }
  ret += "]";
  return ret;
}
