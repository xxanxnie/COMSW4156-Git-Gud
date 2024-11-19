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
  std::vector<std::pair<std::string, std::string>> expectedContent =
      shelter->createDBContent("tmp", "tmp", "tmp", "1", "0");
  ON_CALL(*mockDbManager, insertResource(::testing::_, ::testing::_))
      .WillByDefault(
          [&](const std::string& collectionName,
              const std::vector<std::pair<std::string, std::string>>& content) {
            EXPECT_EQ(collectionName, "ShelterTest");
            EXPECT_EQ(content, expectedContent);
            return "12345";
          });
  std::string ret = shelter->addShelter(
      "{\"rwe\":\"tmp\",\"wre\": \"tmp\", \"wer\":\"tmp\",\"wer\": \"1\", \"rer\":\"0\"}");
  EXPECT_EQ(ret, "12345");
}

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

TEST_F(ShelterUnitTests, UpdateShelter) {
  std::vector<std::pair<std::string, std::string>> expectedContent =
      shelter->createDBContent("tmp", "tmp", "tmp", "1", "0");
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
  std::string ret = shelter->updateShelter(id_temp, "tmp", "tmp", "tmp", 1, 0);
  EXPECT_EQ(ret, "Update");
}

TEST_F(ShelterUnitTests, DeleteShelter) {
  std::string id_temp = "123456789";
  ON_CALL(*mockDbManager, deleteResource(::testing::_, ::testing::_))
      .WillByDefault([&](const std::string& collectionName,
                         const std::string& resourceId) {
        EXPECT_EQ(resourceId, id_temp);
        EXPECT_EQ(collectionName, "ShelterTest");
        return 1;
      });
  std::string ret = shelter->deleteShelter(id_temp);
  EXPECT_EQ(ret, "SUC");
}
