// Copyright 2024 COMSW4156-Git-Gud

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include "Shelter.h"
class MockDatabaseManager : public DatabaseManager {
 public:
  MockDatabaseManager() : DatabaseManager("mongodb://localhost:27017", true) {}

  MOCK_METHOD(
      void, findCollection,
      (const std::string& collectionName,
       (const std::vector<std::pair<std::string, std::string>>& keyValues),
       (std::vector<bsoncxx::document::value> & result)),
      (override));

  MOCK_METHOD(
      void, insertResource,
      (const std::string& collectionName,
       (const std::vector<std::pair<std::string, std::string>>& keyValues)),
      (override));
};
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

TEST_F(ShelterUnitTests, searchShelterAll) {
  std::vector<bsoncxx::document::value> mockResult;
  mockResult.push_back(bsoncxx::builder::stream::document{}
                       << "ORG" << "NGO"
                       << "User" << "HML"
                       << "location" << "New York"
                       << "capacity" << "10"
                       << "curUse" << "0"
                       << bsoncxx::builder::stream::finalize);

  ON_CALL(*mockDbManager,
          findCollection(::testing::_, ::testing::_, ::testing::_))
      .WillByDefault(::testing::DoAll(::testing::SetArgReferee<2>(mockResult),
                                      ::testing::Return()));

  std::string services = shelter->searchShelterAll();
  EXPECT_NE(services, "[]");
  EXPECT_TRUE(services.find("NGO") != std::string::npos);
  EXPECT_TRUE(services.find("HML") != std::string::npos);
  EXPECT_TRUE(services.find("New York") != std::string::npos);
}
TEST_F(ShelterUnitTests, AddNewShelter) {
  std::vector<std::pair<std::string, std::string>> expectedContent =
      shelter->createDBContent("tmp", "tmp", "tmp", "1", "0");
  ON_CALL(*mockDbManager, insertResource(::testing::_, ::testing::_))
      .WillByDefault(
          [&](const std::string& collectionName,
              const std::vector<std::pair<std::string, std::string>>& content) {
            EXPECT_EQ(collectionName, "ShelterTest");
            EXPECT_EQ(content, expectedContent);
          });
  std::string ret = shelter->addShelter("tmp", "tmp", "tmp", 1, 0);
  EXPECT_EQ(ret, "Success");
}
