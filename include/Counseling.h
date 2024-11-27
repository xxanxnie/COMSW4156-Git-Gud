#ifndef COUNSELING_H
#define COUNSELING_H

#include <bsoncxx/document/value.hpp>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class DatabaseManager;

class Counseling {
 public:
  Counseling(DatabaseManager& dbManager, const std::string& collection_name);
  void cleanCache();
  std::string checkInputFormat(std::string content);
  virtual std::string addCounselor(std::string request_body);
  virtual std::string deleteCounselor(const std::string& counselorId);
  virtual std::string searchCounselorsAll(int start = 0);
  virtual std::string updateCounselor(std::string request_body);
  std::vector<std::pair<std::string, std::string>> createDBContent();

 private:
  DatabaseManager& dbManager;
  std::string collection_name;
  std::vector<std::string> cols;
  std::unordered_map<std::string, std::string> format;

  std::string getCounselorID(const bsoncxx::document::view& counselor);
  std::string printCounselors(
      std::vector<bsoncxx::document::value>& counselors) const;
};

#endif
