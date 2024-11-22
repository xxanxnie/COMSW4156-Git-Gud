// Copyright 2024 Annie, Xu

#ifndef OUTREACH_H
#define OUTREACH_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "DatabaseManager.h"

class Outreach {
 public:
  Outreach(DatabaseManager& dbManager, const std::string& collection_name);

  std::string collection_name;
  void cleanCache();
  std::string checkInputFormat(std::string content);
  std::unordered_map<std::string, std::string> format;
  virtual std::string addOutreachService(std::string request_body);

  std::vector<std::pair<std::string, std::string>> createDBContent();

  virtual std::string getAllOutreachServices(int start = 0);
  virtual std::string deleteOutreach(std::string id);
  virtual std::string updateOutreach(std::string request_body);

  std::string printOutreachServices(
      const std::vector<bsoncxx::document::value>& services) const;

 private:
  DatabaseManager& dbManager;
  std::vector<std::string> cols;
};

#endif  // OUTREACH_H
