#pragma once

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <optional>
#include <string>
#include <vector>

class DatabaseManager {
 public:
  DatabaseManager(const std::string& uri, bool skipInitialization = false);

  virtual ~DatabaseManager() = default;

  virtual void createCollection(const std::string& collectionName);
  virtual void printCollection(const std::string& collectionName);
  virtual void findCollection(
      int start, const std::string& collectionName,
      const std::vector<std::pair<std::string, std::string>>& keyValues,
      std::vector<bsoncxx::document::value>& result);
  virtual std::string insertResource(
      const std::string& collectionName,
      const std::vector<std::pair<std::string, std::string>>& keyValues);
  virtual bool deleteResource(const std::string& collectionName,
                              const std::string& resourceId,
                              const std::string &authToken);
  virtual void deleteCollection(const std::string& collectionName);
  virtual void updateResource(
      const std::string& collectionName, const std::string& resourceId,
      const std::vector<std::pair<std::string, std::string>>& updates);
  virtual void findResource(const std::string& collectionName,
                            const std::string& resourceId);
  virtual bsoncxx::document::value getResources(
      const std::string& resourceType);

  static DatabaseManager& getInstance() {
    static DatabaseManager instance("mongodb://localhost:27017");
    return instance;
  }

 protected:
  std::optional<mongocxx::client> conn;

  bsoncxx::document::value createDocument(
      const std::vector<std::pair<std::string, std::string>>& keyValues);
};
