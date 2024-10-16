// DatabaseManager.cpp
#include "DatabaseManager.h"

#include <algorithm>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <iostream>

DatabaseManager::DatabaseManager(const std::string& uri)
    : conn(mongocxx::uri{uri}) {}

bsoncxx::document::value DatabaseManager::createDocument(
    const std::vector<std::pair<std::string, std::string>>& keyValues) {
  bsoncxx::builder::stream::document document{};
  for (const auto& keyValue : keyValues) {
    document << keyValue.first << keyValue.second;
  }
  return document << bsoncxx::builder::stream::finalize;
}

void DatabaseManager::createCollection(const std::string& collectionName) {
  try {
    conn["GitGud"][collectionName];  // Create collection if it doesn't exist
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
}
void DatabaseManager::findCollection(
    const std::string& collectionName,
    const std::vector<std::pair<std::string, std::string>>& keyValues,
    std::vector<bsoncxx::document::view>& result) {
  try {
    auto collection = conn["GitGud"][collectionName];
    auto cursor = collection.find(createDocument(keyValues).view());
    for (auto&& doc : cursor) {
    //   std::cout << bsoncxx::to_json(doc) << std::endl;
      result.push_back(doc);
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
}
void DatabaseManager::printCollection(const std::string& collectionName) {
  try {
    auto collection = conn["GitGud"][collectionName];
    if (collection.count_documents({}) == 0) {
      std::cout << "Collection " << collectionName << " is empty." << std::endl;
      return;
    }

    auto cursor = collection.find({});
    for (auto&& doc : cursor) {
      std::cout << bsoncxx::to_json(doc) << std::endl;
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
}

void DatabaseManager::insertResource(
    const std::string& collectionName,
    const std::vector<std::pair<std::string, std::string>>& keyValues) {
  try {
    auto collection = conn["GitGud"][collectionName];
    collection.insert_one(createDocument(keyValues).view());
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n' << "error";
  }
}

void DatabaseManager::deleteResource(const std::string& collectionName,
                                     const std::string& resourceId) {
  try {
    auto collection = conn["GitGud"][collectionName];
    collection.delete_one(createDocument({{"_id", resourceId}}).view());
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n' << "error";
  }
}

void DatabaseManager::updateResource(
    const std::string& collectionName, const std::string& resourceId,
    const std::vector<std::pair<std::string, std::string>>& updates) {
  try {
    auto collection = conn["GitGud"][collectionName];
    bsoncxx::builder::stream::document updateDoc{};
    updateDoc << "$set" << bsoncxx::builder::stream::open_document;
    for (const auto& update : updates) {
      updateDoc << update.first << update.second;
    }
    updateDoc << bsoncxx::builder::stream::close_document;

    collection.update_one(bsoncxx::builder::stream::document{}
                              << "_id" << resourceId
                              << bsoncxx::builder::stream::finalize,
                          updateDoc.view());
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n' << "error";
  }
}

void DatabaseManager::findResource(const std::string& collectionName,
                                   const std::string& resourceId) {
  try {
    auto collection = conn["GitGud"][collectionName];
    auto filter = bsoncxx::builder::stream::document{}
                  << "_id" << resourceId << bsoncxx::builder::stream::finalize;
    auto cursor = collection.find(filter.view());
    for (auto&& doc : cursor) {
      std::cout << bsoncxx::to_json(doc) << std::endl;
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n' << "error";
  }
}

bsoncxx::document::value DatabaseManager::getResources(
    const std::string& resourceType) {
  auto collection = conn["GitGud"]["Resources"];
  auto filter = bsoncxx::builder::stream::document{}
                << "type" << resourceType << bsoncxx::builder::stream::finalize;
  auto cursor = collection.find(filter.view());

  using bsoncxx::builder::stream::array;
  using bsoncxx::builder::stream::document;
  using bsoncxx::builder::stream::finalize;

  auto result = document{};
  auto resources_array = array{};

  for (auto&& doc : cursor) {
    resources_array << bsoncxx::builder::stream::open_document
                    << bsoncxx::builder::stream::concatenate(doc)
                    << bsoncxx::builder::stream::close_document;
  }

  result << "resources" << resources_array;

  return result << finalize;
}
