#ifndef HEALTHCARE_H
#define HEALTHCARE_H

#include <string>
#include <unordered_map>
#include <vector>

#include "DatabaseManager.h"

class Healthcare {
 public:
  std::string collection_name;

  Healthcare(DatabaseManager& dbManager, const std::string& collection_name);
  void cleanCache();
  std::string checkInputFormat(std::string content);
  virtual std::string addHealthcareService(std::string request_body);

  virtual std::string getAllHealthcareServices();

  virtual std::string deleteHealthcare(std::string id);
  virtual std::string updateHealthcare(std::string request_body);

  virtual std::string validateHealthcareServiceInput(
      const std::map<std::string, std::string>& content);

  std::vector<std::pair<std::string, std::string>> createDBContent();

  std::string printHealthcareServices(
      std::vector<bsoncxx::document::value>& services) const;
  std::unordered_map<std::string, std::string> format;

 private:
  DatabaseManager& dbManager;
  std::vector<std::string> cols;
};

#endif
