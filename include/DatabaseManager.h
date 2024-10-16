#pragma once
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <string>
#include <vector>

class DatabaseManager {
 public:
  DatabaseManager(const std::string& uri);

  void createCollection(const std::string& collectionName);
  void printCollection(const std::string& collectionName);
  void findCollection(
      const std::string& collectionName,
      const std::vector<std::pair<std::string, std::string>>& keyValues,
      std::vector < bsoncxx::document::value > &result);
  void insertResource(
      const std::string& collectionName,
      const std::vector<std::pair<std::string, std::string>>& keyValues);
  void deleteResource(const std::string& collectionName,
                      const std::string& resourceId);
  void updateResource(
      const std::string& collectionName, const std::string& resourceId,
      const std::vector<std::pair<std::string, std::string>>& updates);
  void findResource(const std::string& collectionName,
                    const std::string& resourceId);
  void deleteDocument(const std::string& collectionName,
                      const std::string& resourceId);
  bsoncxx::document::value getResources(const std::string& resourceType);

 private:
  mongocxx::client conn;

  bsoncxx::document::value createDocument(
      const std::vector<std::pair<std::string, std::string>>& keyValues);
};
