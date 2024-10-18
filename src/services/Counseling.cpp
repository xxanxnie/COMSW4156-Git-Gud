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

Counseling::Counseling(DatabaseManager &dbManager)
    : dbManager(dbManager), collection_name("Counseling") {}

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

std::vector<std::pair<std::string, std::string>> Counseling::createDBContent(
    const std::string &counselorName, const std::string &specialty) {
  std::vector<std::pair<std::string, std::string>> content;
  content.push_back({"counselorName", counselorName});
  content.push_back({"specialty", specialty});
  return content;
}

std::string Counseling::deleteCounselor(const std::string &counselorId) {
  // TODO(brendon): Implement counselor deletion logic
  return "Delete";
}

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

std::string Counseling::updateCounselor(const std::string &counselorId, const std::string &field, const std::string &value) {
  // TODO(brendon): Implement counselor update logic
  return "Update";
}

std::string Counseling::getCounselorID(const bsoncxx::document::view &counselor) {
  std::string id = counselor["_id"].get_oid().value.to_string();
  std::cout << id << std::endl;
  return id;
}

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
