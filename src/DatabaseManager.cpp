// Copyright 2024 COMSW4156-Git-Gud

#include "DatabaseManager.h"

#include <algorithm>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>

DatabaseManager::DatabaseManager(const std::string &uri,
                                 bool skipInitialization) {
  if (!skipInitialization) {
    conn = mongocxx::client(mongocxx::uri{uri});
  } else {
    // This is only needed for allowing unit-tests
    conn.reset();
  }
}

bsoncxx::document::value DatabaseManager::createDocument(
    const std::vector<std::pair<std::string, std::string>> &keyValues) {
  bsoncxx::builder::stream::document document{};
  for (const auto &keyValue : keyValues) {
    document << keyValue.first << keyValue.second;
  }
  return document << bsoncxx::builder::stream::finalize;
}

void DatabaseManager::createCollection(const std::string &collectionName) {
  (*conn)["GitGud"][collectionName];
}

void DatabaseManager::findCollection(
    const std::string &collectionName,
    const std::vector<std::pair<std::string, std::string>> &keyValues,
    std::vector<bsoncxx::document::value> &result) {
  auto collection = (*conn)["GitGud"][collectionName];
  auto cursor = collection.find(createDocument(keyValues).view());

  for (auto &&doc : cursor) {
    result.push_back(bsoncxx::document::value(doc));
  }
}

void DatabaseManager::printCollection(const std::string &collectionName) {
  auto collection = (*conn)["GitGud"][collectionName];
  if (collection.count_documents({}) == 0) {
    std::cout << "Collection " << collectionName << " is empty." << std::endl;
    return;
  }

  auto cursor = collection.find({});
  for (auto &&doc : cursor) {
    std::cout << bsoncxx::to_json(doc) << std::endl;
  }
}

void DatabaseManager::insertResource(
    const std::string &collectionName,
    const std::vector<std::pair<std::string, std::string>> &keyValues) {
  auto collection = (*conn)["GitGud"][collectionName];
  collection.insert_one(createDocument(keyValues).view());
}

bool DatabaseManager::deleteResource(const std::string &collectionName,
                                     const std::string &resourceId) {
  auto collection = (*conn)["GitGud"][collectionName];

  bsoncxx::builder::stream::document filter_builder;
  bsoncxx::oid oid(resourceId);
  filter_builder << "_id" << oid;

  auto result = collection.delete_one(filter_builder.view());
  if (result && result->deleted_count() > 0) {
      std::cout << "Document deleted successfully.\n";
      return 1;
  } else {
      std::cout << "No document found with the given _id.\n";
      return 0;
  }
}

void DatabaseManager::deleteCollection(const std::string &collectionName) {
  auto collection = (*conn)["GitGud"][collectionName];
  collection.drop();
}

void DatabaseManager::updateResource(
    const std::string &collectionName, const std::string &resourceId,
    const std::vector<std::pair<std::string, std::string>> &updates) {
  auto collection = (*conn)["GitGud"][collectionName];
  bsoncxx::builder::stream::document updateDoc{};
  updateDoc << "$set" << bsoncxx::builder::stream::open_document;

  for (const auto &update : updates) {
    updateDoc << update.first << update.second;
  }
  updateDoc << bsoncxx::builder::stream::close_document;

  collection.update_one(bsoncxx::builder::stream::document{}
                            << "_id" << resourceId
                            << bsoncxx::builder::stream::finalize,
                        updateDoc.view());
}

void DatabaseManager::findResource(const std::string &collectionName,
                                   const std::string &resourceId) {
  auto collection = (*conn)["GitGud"][collectionName];
  auto filter = bsoncxx::builder::stream::document{}
                << "_id" << resourceId << bsoncxx::builder::stream::finalize;
  auto cursor = collection.find(filter.view());

  for (auto &&doc : cursor) {
    std::cout << bsoncxx::to_json(doc) << std::endl;
  }
}

bsoncxx::document::value DatabaseManager::getResources(
    const std::string &resourceType) {
  auto collection = (*conn)["GitGud"]["Resources"];
  auto filter = bsoncxx::builder::stream::document{}
                << "type" << resourceType << bsoncxx::builder::stream::finalize;
  auto cursor = collection.find(filter.view());

  using bsoncxx::builder::stream::array;
  using bsoncxx::builder::stream::document;
  using bsoncxx::builder::stream::finalize;

  auto result = document{};
  auto resources_array = array{};

  for (auto &&doc : cursor) {
    resources_array << bsoncxx::builder::stream::open_document
                    << bsoncxx::builder::stream::concatenate(doc)
                    << bsoncxx::builder::stream::close_document;
  }

  result << "resources" << resources_array;

  return result << finalize;
}