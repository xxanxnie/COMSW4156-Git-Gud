#ifndef HEALTHCARE_H
#define HEALTHCARE_H

#include <string>
#include <vector>

#include "DatabaseManager.h"

class Healthcare {
 public:
  std::string collection_name;

  Healthcare(DatabaseManager& dbManager, const std::string& collection_name)
      : dbManager(dbManager), collection_name(collection_name) {}

  virtual std::string addHealthcareService(const std::map<std::string, std::string>& updates);

  virtual std::string getAllHealthcareServices();

  virtual std::string deleteHealthcare(std::string id);
  virtual std::string updateHealthcare(
      const std::string& id, const std::map<std::string, std::string>& updates
  );

  virtual std::string validateHealthcareServiceInput(
    const std::map<std::string, std::string>& content);

  std::vector<std::pair<std::string, std::string>> createDBContent(
      const std::map<std::string, std::string>& updates);

  std::string printHealthcareServices(
      std::vector<bsoncxx::document::value>& services) const;

 private:
  DatabaseManager& dbManager;
};

#endif
