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

  ON_CALL(*mockDbManager,
          findCollection(::testing::_, ::testing::_, ::testing::_))
      .WillByDefault(::testing::DoAll(::testing::SetArgReferee<2>(mockResult),
                                      ::testing::Return()));

  std::string counselors = counseling->searchCounselorsAll();

  EXPECT_NE(counselors, "[]");
  EXPECT_TRUE(counselors.find("John Doe") != std::string::npos);
  EXPECT_TRUE(counselors.find("Cognitive Behavioral Therapy") !=
              std::string::npos);
}

TEST_F(CounselingUnitTests, AddCounselor) {
  std::vector<std::pair<std::string, std::string>> expectedContent = {
      {"counselorName", "Jane Smith"}, {"specialty", "Family Therapy"}};

  ON_CALL(*mockDbManager, insertResource(::testing::_, ::testing::_))
      .WillByDefault(::testing::Invoke(
          [&](const std::string &collectionName,
              const std::vector<std::pair<std::string, std::string>> &content) {
            EXPECT_EQ(collectionName, "Counseling");
            EXPECT_EQ(content, expectedContent);
            return "12345";
          }));

  std::string result = counseling->addCounselor("Jane Smith", "Family Therapy");

  EXPECT_EQ(result, "12345");
}

// TEST_F(CounselingUnitTests, UpdateCounselor) {
//     std::string id = "12345";
//     std::string field = "specialty";
//     std::string value = "Trauma Therapy";

//     std::vector<std::pair<std::string, std::string>> expectedUpdates = {
//         {field, value}
//     };

//     ON_CALL(*mockDbManager, updateResource(::testing::_, ::testing::_,
//     ::testing::_))
//         .WillByDefault(::testing::Invoke(
//             [&](const std::string& collectionName,
//                 const std::string& resourceId,
//                 const std::vector<std::pair<std::string, std::string>>&
//                 updates) { EXPECT_EQ(collectionName, "Counseling");
//                 EXPECT_EQ(resourceId, id);
//                 EXPECT_EQ(updates, expectedUpdates);
//             }
//         ));

//     std::string result = counseling->updateCounselor(id, field, value);

//     EXPECT_EQ(result, "Update");
// }

// TEST_F(CounselingUnitTests, DeleteCounselor) {
//     std::string id = "12345";

//     ON_CALL(*mockDbManager, deleteResource(::testing::_, ::testing::_))
//         .WillByDefault(::testing::Invoke(
//             [&](const std::string& collectionName, const std::string&
//             resourceId) {
//                 EXPECT_EQ(collectionName, "Counseling");
//                 EXPECT_EQ(resourceId, id);
//             }
//         ));

//     std::string result = counseling->deleteCounselor(id);

//     EXPECT_EQ(result, "Delete");
// }
