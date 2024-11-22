// Copyright 2024 COMSW4156-Git-Gud

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include "Counseling.h"
#include "MockDatabaseManager.h"

class CounselingUnitTests : public ::testing::Test {
 protected:
  MockDatabaseManager *mockDbManager;
  Counseling *counseling;

  void SetUp() override {
    mockDbManager = new MockDatabaseManager();
    counseling = new Counseling(*mockDbManager);
  }

  void TearDown() override {
    delete counseling;
    delete mockDbManager;
  }
};

TEST_F(CounselingUnitTests, SearchCounselorsAll) {
  std::vector<bsoncxx::document::value> mockResult;
  mockResult.push_back(bsoncxx::builder::stream::document{}
                       << "name" << "John Doe"
                       << "specialty" << "Cognitive Behavioral Therapy"
                       << bsoncxx::builder::stream::finalize);

  ON_CALL(*mockDbManager, findCollection(::testing::_, ::testing::_,
                                         ::testing::_, ::testing::_))
      .WillByDefault(::testing::DoAll(::testing::SetArgReferee<3>(mockResult),
                                      ::testing::Return()));

  std::string counselors = counseling->searchCounselorsAll(0);

  EXPECT_NE(counselors, "[]");
  EXPECT_TRUE(counselors.find("John Doe") != std::string::npos);
  EXPECT_TRUE(counselors.find("Cognitive Behavioral Therapy") !=
              std::string::npos);
}

TEST_F(CounselingUnitTests, AddCounselor) {
  std::string input = R"({
        "Name" : "OrganicFarm",
        "City" : "New York",
        "Address": "temp",
        "Description" : "Vegetables",
        "ContactInfo" : "66664566565",
        "HoursOfOperation": "2024-01-11",
        "counselorName": "Jack"
    })";
  counseling->checkInputFormat(input);
  std::vector<std::pair<std::string, std::string>> expectedContent =
      counseling->createDBContent();

  ON_CALL(*mockDbManager, insertResource(::testing::_, ::testing::_))
      .WillByDefault(::testing::Invoke(
          [&](const std::string &collectionName,
              const std::vector<std::pair<std::string, std::string>> &content) {
            EXPECT_EQ(collectionName, "Counseling");
            EXPECT_EQ(content, expectedContent);
            return "12345";
          }));

  std::string result = counseling->addCounselor(input);

  EXPECT_EQ(result, "12345");
}

TEST_F(CounselingUnitTests, updateCounseling) {
  std::string input =
      R"({
      "id":"123456789" ,
    "Name" : "OrganicFarm",
        "City" : "New York",
        "Address": "temp",
        "Description" : "Vegetables",
        "ContactInfo" : "66664566565",
        "HoursOfOperation": "2024-01-11",
        "counselorName": "Jack"
  })";
  counseling->checkInputFormat(input);
  std::vector<std::pair<std::string, std::string>> expectedContent =
      counseling->createDBContent();
  std::string id_temp = "123456789";
  ON_CALL(*mockDbManager,
          updateResource(::testing::_, ::testing::_, ::testing::_))
      .WillByDefault(
          [&](const std::string &collectionName, const std::string &resourceId,
              const std::vector<std::pair<std::string, std::string>> &content)
              -> bool {
            EXPECT_EQ(resourceId, id_temp);
            EXPECT_EQ(collectionName, "Counseling");
            EXPECT_EQ(content, expectedContent);
            return true;
          });

  std::string result = counseling->updateCounselor(input);
  EXPECT_EQ(result, "Success");

  // Verify the updated data can be retrieved
  std::vector<bsoncxx::document::value> mockResult;
  mockResult.push_back(bsoncxx::builder::stream::document{}
                       << "Name"
                       << "OrganicFarm" << "City" << "New York" << "Address"
                       << "temp" << "Description" << "Vegetables"
                       << "ContactInfo" << "66664566565" << "HoursOfOperation"
                       << "2024-01-11" << "counselorName" << "Jack"
                       << bsoncxx::builder::stream::finalize);

  ON_CALL(*mockDbManager, findCollection(::testing::_, ::testing::_,
                                         ::testing::_, ::testing::_))
      .WillByDefault(::testing::DoAll(::testing::SetArgReferee<3>(mockResult),
                                      ::testing::Return()));

  std::string counselingItems = counseling->searchCounselorsAll(0);
  EXPECT_TRUE(counselingItems.find("OrganicFarm") != std::string::npos);
  EXPECT_TRUE(counselingItems.find("Jack") != std::string::npos);
  EXPECT_TRUE(counselingItems.find("2024-01-11") != std::string::npos);
}

TEST_F(CounselingUnitTests, DeleteCounselor) {
  std::string mockId = "123";

  ON_CALL(*mockDbManager, deleteResource(::testing::_, ::testing::_))
      .WillByDefault([&](const std::string &collectionName,
                         const std::string &resourceId) -> bool {
        EXPECT_EQ(resourceId, mockId);
        EXPECT_EQ(collectionName, "Counseling");
        return true;
      });

  std::string result = counseling->deleteCounselor(mockId);
  EXPECT_EQ(result, "Success");

  // Verify deletion by checking empty results
  std::vector<bsoncxx::document::value> mockResult;
  ON_CALL(*mockDbManager, findCollection(::testing::_, ::testing::_,
                                         ::testing::_, ::testing::_))
      .WillByDefault(::testing::DoAll(::testing::SetArgReferee<3>(mockResult),
                                      ::testing::Return()));

  std::string counselingItems = counseling->searchCounselorsAll(0);
  EXPECT_EQ(counselingItems, "[]");
}
