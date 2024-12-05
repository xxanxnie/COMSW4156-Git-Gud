// Copyright 2024 COMSW4156-Git-Gud

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include "MockDatabaseManager.h"
#include "Shelter.h"

class ShelterUnitTests : public ::testing::Test {
 protected:
  Shelter* shelter;
  MockDatabaseManager* mockDbManager;
  void SetUp() override {
    mockDbManager = new MockDatabaseManager();
    shelter = new Shelter(*mockDbManager, "ShelterTest");
  }
  void TearDown() override {
    delete shelter;
    delete mockDbManager;
  }
};
TEST_F(ShelterUnitTests, AddNewShelter) {
  shelter->checkInputFormat(
      "{\"Name\" : \"temp\",\"City\" : \"New York\",\"Address\": "
      "\"temp\",\"Description\" : \"NULL\",\"ContactInfo\" : "
      "\"66664566565\",\"HoursOfOperation\": "
      "\"2024-01-11\",\"ORG\":\"NGO\",\"TargetUser\" "
      ":\"HML\",\"Capacity\" : \"100\",\"CurrentUse\": \"10\"}",
      "456");
  std::vector<std::pair<std::string, std::string>> expectedContent =
      shelter->createDBContent();
  ON_CALL(*mockDbManager, insertResource(::testing::_, ::testing::_))
      .WillByDefault(
          [&](const std::string& collectionName,
              const std::vector<std::pair<std::string, std::string>>& content) {
            EXPECT_EQ(collectionName, "ShelterTest");
            EXPECT_EQ(content, expectedContent);
            return "12345";
          });
  std::string ret = shelter->addShelter(
      "{\"Name\" : \"temp\",\"City\" : \"New York\",\"Address\": "
      "\"temp\",\"Description\" : \"NULL\",\"ContactInfo\" : "
      "\"66664566565\",\"HoursOfOperation\": "
      "\"2024-01-11\",\"ORG\":\"NGO\",\"TargetUser\" "
      ":\"HML\",\"Capacity\" : \"100\",\"CurrentUse\": \"10\"}",
      "456");
  EXPECT_EQ(ret, "12345");
}

TEST_F(ShelterUnitTests, searchShelterAll) {
  std::vector<bsoncxx::document::value> mockResult;
  mockResult.push_back(bsoncxx::builder::stream::document{}
                       << "Name" << "temp" << "City" << "New York" << "Address"
                       << "temp" << "Description" << "NULL" << "ContactInfo"
                       << "66664566565" << "HoursOfOperation" << "2024-01-11"
                       << "ORG" << "NGO" << "TargetUser" << "HML"
                       << "Capacity" << "100" << "CurrentUse" << "10"
                       << bsoncxx::builder::stream::finalize);

  ON_CALL(*mockDbManager, findCollection(::testing::_, ::testing::_,
                                         ::testing::_, ::testing::_))
      .WillByDefault(::testing::DoAll(::testing::SetArgReferee<3>(mockResult),
                                      ::testing::Return()));

  std::string services = shelter->searchShelterAll();
  EXPECT_NE(services, "[]");
  EXPECT_TRUE(services.find("NGO") != std::string::npos);
  EXPECT_TRUE(services.find("HML") != std::string::npos);
  EXPECT_TRUE(services.find("New York") != std::string::npos);
}

TEST_F(ShelterUnitTests, UpdateShelter) {
  shelter->checkInputFormat(R"({
        "id":"123456789" ,
        "CurrentUse" : "10", "Capacity" : "100", 
        "TargetUser" : "HML", "ORG" : "NGO", 
        "HoursOfOperation" : "2024-01-11", 
        "ContactInfo" : "66664566565", "Description" : "NULL", 
        "Address" : "temp", "City" : "New York", "Name" : "temp"
        })",
                            "456");
  std::vector<std::pair<std::string, std::string>> expectedContent =
      shelter->createDBContent();
  std::string id_temp = "123456789";
  ON_CALL(*mockDbManager,
          updateResource(::testing::_, ::testing::_, ::testing::_))
      .WillByDefault(
          [&](const std::string& collectionName, const std::string& resourceId,
              const std::vector<std::pair<std::string, std::string>>& content) {
            EXPECT_EQ(resourceId, id_temp);
            EXPECT_EQ(collectionName, "ShelterTest");
            EXPECT_EQ(content, expectedContent);
          });
  std::string ret = shelter->updateShelter(
      R"({
        "id":"123456789" ,
        "CurrentUse" : "10", "Capacity" : "100", 
        "TargetUser" : "HML", "ORG" : "NGO", 
        "HoursOfOperation" : "2024-01-11", 
        "ContactInfo" : "66664566565", "Description" : "NULL", 
        "Address" : "temp", "City" : "New York", "Name" : "temp"
        })",
      "456");
  EXPECT_EQ(ret, "Update");
}

TEST_F(ShelterUnitTests, DeleteShelter) {
  std::string id_temp = "123456789";
  ON_CALL(*mockDbManager,
          deleteResource(::testing::_, ::testing::_, ::testing::_))
      .WillByDefault([&](const std::string& collectionName,
                         const std::string& resourceId,
                         const std::string& authToken) {
        EXPECT_EQ(resourceId, id_temp);
        EXPECT_EQ(collectionName, "ShelterTest");
        return 1;
      });
  std::string ret = shelter->deleteShelter(id_temp, "456");
  EXPECT_EQ(ret, "SUC");
}
