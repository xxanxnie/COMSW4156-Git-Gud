#ifndef HEALTHCARE_H
#define HEALTHCARE_H

#include <string>
#include <vector>

#include "DatabaseManager.h"

class Healthcare {
 public:
  std::string collection_name;

  Healthcare(DatabaseManager& dbManager,
                    const std::string& collection_name)
      : dbManager(dbManager), collection_name(collection_name) {}

  virtual std::string addHealthcareService(const std::string& provider,
                                   const std::string& serviceType,
                                   const std::string& location,
                                   const std::string& operatingHours,
                                   const std::string& eligibilityCriteria,
                                   const std::string& contactInfo);

  virtual std::string getAllHealthcareServices();

  std::vector<std::pair<std::string, std::string>> createDBContent(
      const std::string& provider, const std::string& serviceType,
      const std::string& location, const std::string& operatingHours,
      const std::string& eligibilityCriteria, const std::string& contactInfo);

  std::string printHealthcareServices(
      std::vector<bsoncxx::document::value>& services) const;

 private:
  DatabaseManager& dbManager;
};

#endif
