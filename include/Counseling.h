#ifndef COUNSELING_H
#define COUNSELING_H

#include <bsoncxx/document/value.hpp>
#include <string>
#include <utility>
#include <vector>

class DatabaseManager;

class Counseling {
 public:
  Counseling(DatabaseManager& dbManager);

  virtual std::string addCounselor(const std::string& counselorName,
                                   const std::string& specialty);
  virtual std::string deleteCounselor(const std::string& counselorId);
  virtual std::string searchCounselorsAll();
  virtual std::string updateCounselor(const std::string& counselorId,
                                      const std::string& field,
                                      const std::string& value);

 private:
  DatabaseManager& dbManager;
  std::string collection_name;

  std::vector<std::pair<std::string, std::string>> createDBContent(
      const std::string& counselorName, const std::string& specialty);
  std::string getCounselorID(const bsoncxx::document::view& counselor);
  std::string printCounselors(
      std::vector<bsoncxx::document::value>& counselors) const;
};

#endif
