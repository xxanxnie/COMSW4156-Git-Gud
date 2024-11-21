// Copyright 2024 COMSW4156-Git-Gud
#ifndef MOCK_DATABASE_MANAGER_H
#define MOCK_DATABASE_MANAGER_H

#include <gmock/gmock.h>

#include "DatabaseManager.h"

class MockDatabaseManager : public DatabaseManager {
 public:
  MockDatabaseManager() : DatabaseManager("mongodb://localhost:27017", true) {}

  MOCK_METHOD(
      void, findCollection,
      (const std::string &collectionName,
       (const std::vector<std::pair<std::string, std::string>> &keyValues),
       (std::vector<bsoncxx::document::value> & result)),
      (override));

  MOCK_METHOD(
      std::string, insertResource,
      (const std::string &collectionName,
       (const std::vector<std::pair<std::string, std::string>> &keyValues)),
      (override));

  MOCK_METHOD(
      void, updateResource,
      (const std::string &collectionName, (const std::string &id),
       (const std::vector<std::pair<std::string, std::string>> &updates)),
      (override));

  MOCK_METHOD(bool, deleteResource,
      (const std::string& collectionName,
      (const std::string& id)), (override));
};

#endif  // MOCK_DATABASE_MANAGER_H
