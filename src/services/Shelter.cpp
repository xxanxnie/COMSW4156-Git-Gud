// Copyright 2024 Wilson, Liang
#include "Shelter.h"

/**
 * Add the shelter information to our database
 *
 * @param ORG     The organization who gave the infomation.
 * @param User     The target this service provide for.
 * @param location     The location of the shelter.
 * @param capacity     The maximum number of users that the shelter can handle.
 * @param curUse       The current users that using this shelter.
 * @return code
 */
std::string Shelter::addShelter(std::string ORG, std::string User,
                                std::string location, int capacity,
                                int curUse) {
  auto content = createDBContent(ORG, User, location, std::to_string(capacity),
                                 std::to_string(curUse));
  try {
    dbManager.insertResource(collection_name, content);
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
  return "Success";
}
std::vector<std::pair<std::string, std::string>> Shelter::createDBContent(
    std::string ORG, std::string User, std::string location,
    std::string capacity, std::string curUse) {
  std::vector<std::pair<std::string, std::string>> content;
  content.push_back({"ORG", ORG});
  content.push_back({"User", User});
  content.push_back({"location", location});
  content.push_back({"capacity", capacity});
  content.push_back({"curUse", curUse});
  return content;
}
std::string Shelter::updateShelter() { return "Update"; }
std::string Shelter::deleteShelter() { return "Delete"; }
std::string Shelter::searchShelterAll() {
  std::vector<bsoncxx::document::view> result;
  dbManager.findCollection(collection_name, {}, result);
  std::string ret;
  if (result.size() > 0) {
    ret = printShelters(result);
    getShelterID(result[0]);
  }
  return ret;
}
std::string Shelter::getShelterID(bsoncxx::document::view &shelter) {
  std::string id = shelter["_id"].get_oid().value.to_string();
  std::cout << id << std::endl;
  return id;
}
std::string Shelter::printShelters(
    std::vector<bsoncxx::document::view> &shelters) const {
  std::string ret;
  for (auto shelter : shelters) {
    ret +=bsoncxx::to_json(shelter)+"\n";
  }
  std::cout << ret << std::endl;
  return ret;
}
