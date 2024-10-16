#ifndef COUNSELING_H
#define COUNSELING_H

#include <string>
#include <vector>
#include <utility>
#include <bsoncxx/document/value.hpp>

class DatabaseManager;

class Counseling {
 public:
  Counseling(DatabaseManager& dbManager);

  std::string addCounselor(const std::string& counselorName, const std::string& specialty);
  std::string deleteCounselor(const std::string& counselorId);
  std::string searchCounselorsAll();
  std::string updateCounselor(const std::string& counselorId, const std::string& field, const std::string& value);

 private:
  DatabaseManager& dbManager;
  std::string collection_name;

  std::vector<std::pair<std::string, std::string>> createDBContent(const std::string& counselorName, const std::string& specialty);
  std::string getCounselorID(const bsoncxx::document::view& counselor);
  std::string printCounselors(std::vector<bsoncxx::document::value>& counselors) const;
};

#endif
