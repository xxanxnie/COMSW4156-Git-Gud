// Copyright 2024 COMSW4156-Git-Gud

#include "DatabaseManager.h"

#include <algorithm>
#include <iostream>

#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

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
    int start, const std::string &collectionName,
    const std::vector<std::pair<std::string, std::string>> &keyValues,
    std::vector<bsoncxx::document::value> &result) {
  auto collection = (*conn)["GitGud"][collectionName];
  mongocxx::options::find options;
  options.limit(20);  // Limit results to 20 documents
  options.skip(start);

  std::string excludeField = "authToken";
  bsoncxx::builder::stream::document projectionBuilder;
  projectionBuilder << excludeField
                    << 0;  // Exclude the field by setting it to 0
  options.projection(projectionBuilder.view());

  // Perform the query
  auto cursor = collection.find(createDocument(keyValues).view(), options);

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

std::string DatabaseManager::insertResource(
    const std::string &collectionName,
    const std::vector<std::pair<std::string, std::string>> &keyValues) {
  auto collection = (*conn)["GitGud"][collectionName];
  auto item = collection.insert_one(createDocument(keyValues).view());
  std::cout << item->inserted_id().get_oid().value.to_string() << std::endl;
  return item->inserted_id().get_oid().value.to_string();
}

bool DatabaseManager::deleteResource(const std::string &collectionName,
                                     const std::string &resourceId,
                                     const std::string &authToken) {
  auto collection = (*conn)["GitGud"][collectionName];

  // Build the filter to find the document by _id
  bsoncxx::builder::stream::document filter_builder;
  bsoncxx::oid oid(resourceId);
  filter_builder << "_id" << oid;

  // Retrieve the document first
  auto document = collection.find_one(filter_builder.view());
  if (!document) {
    std::cout << "No document found with the given _id.\n";
    return false;
  }

  // Check if the document contains the expected auth token
  auto doc_view = document->view();
  auto auth_field = doc_view["authToken"];
  if (!auth_field || std::string(auth_field.get_utf8().value) != authToken) {
    std::cout << "Invalid permissions: auth token mismatch.\n";
    return false;
  }

  // Proceed with deletion if the auth token matches
  auto result = collection.delete_one(filter_builder.view());
  if (result && result->deleted_count() > 0) {
    std::cout << "Document deleted successfully.\n";
    return true;
  } else {
    std::cout
        << "Failed to delete document. It might have already been deleted.\n";
    return false;
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
  bsoncxx::oid oid(resourceId);
  auto check = collection.find_one(bsoncxx::builder::stream::document{}
                                   << "_id" << oid
                                   << bsoncxx::builder::stream::finalize);
  if (!check) {
    throw std::invalid_argument(
        "The request with wrong id or invalid permissions.");
  }
  auto result = collection.update_one(bsoncxx::builder::stream::document{}
                                          << "_id" << oid
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