// Copyright 2024 Annie, Xu

#ifndef OUTREACH_H
#define OUTREACH_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "DatabaseManager.h"

class Outreach {
 public:
  Outreach(DatabaseManager& dbManager, const std::string& collection_name)
      : dbManager(dbManager), collection_name(collection_name) {}

  std::string collection_name;

  virtual std::string addOutreachService(const std::string& targetAudience,
                                         const std::string& programName,
                                         const std::string& description,
                                         const std::string& programDate,
                                         const std::string& location,
                                         const std::string& contactInfo);

  std::vector<std::pair<std::string, std::string>> createDBContent(
      const std::string& targetAudience, const std::string& programName,
      const std::string& description, const std::string& programDate,
      const std::string& location, const std::string& contactInfo);

  virtual std::string getAllOutreachServices();
  std::string printOutreachServices(
      const std::vector<bsoncxx::document::value>& services) const;

 private:
  DatabaseManager& dbManager;
};

#endif  // OUTREACH_H