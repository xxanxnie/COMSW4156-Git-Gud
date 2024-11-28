// Copyright 2024 COMSW4156-Git-Gud

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include "DatabaseManager.h"

mongocxx::instance instance{};

class DataBaseTest : public ::testing::Test {
 protected:
  DatabaseManager* DbManager;
  void SetUp() override {
    DbManager = new DatabaseManager("mongodb://localhost:27017");
    DbManager->createCollection("test");
  }

  void TearDown() override {
    DbManager->deleteCollection("test");
    delete DbManager;
  }
};

TEST_F(DataBaseTest, Checksetup) {
  std::vector<bsoncxx::document::value> result;
  DbManager->findCollection(0, "test", {}, result);
  EXPECT_EQ(result.size(), 0);
}

TEST_F(DataBaseTest, InsertResourceTest) {
  std::string id = DbManager->insertResource(
      "test", {{"Name", "Resource A"}, {"Type", "Test"}});
  EXPECT_FALSE(id.empty());

  std::vector<bsoncxx::document::value> result;
  DbManager->findCollection(0, "test", {}, result);
  EXPECT_EQ(result.size(), 1);

  auto doc = bsoncxx::to_json(result[0].view());
  EXPECT_NE(doc.find("Resource A"), std::string::npos);
  EXPECT_NE(doc.find("Test"), std::string::npos);
}

TEST_F(DataBaseTest, DeleteResourceTest) {
  std::string id = DbManager->insertResource(
      "test", {{"Name", "Resource B"}, {"Type", "Test"}});
  EXPECT_FALSE(id.empty());

  bool success = DbManager->deleteResource("test", id);
  EXPECT_TRUE(success);

  std::vector<bsoncxx::document::value> result;
  DbManager->findCollection(0, "test", {}, result);
  EXPECT_EQ(result.size(), 0);
}

TEST_F(DataBaseTest, UpdateResourceTest) {
  std::string id = DbManager->insertResource(
      "test", {{"Name", "Resource C"}, {"Type", "OldType"}});
  EXPECT_FALSE(id.empty());

  DbManager->updateResource("test", id, {{"Type", "NewType"}});

  std::vector<bsoncxx::document::value> result;
  DbManager->findCollection(0, "test", {}, result);
  ASSERT_EQ(result.size(), 1);

  auto doc = bsoncxx::to_json(result[0].view());
  EXPECT_NE(doc.find("Resource C"), std::string::npos);
  EXPECT_NE(doc.find("NewType"), std::string::npos);
}

TEST_F(DataBaseTest, PrintCollectionTest) {
  DbManager->insertResource("test", {{"Name", "Resource E"}, {"Type", "Test"}});
  DbManager->insertResource("test", {{"Name", "Resource F"}, {"Type", "Test"}});

  testing::internal::CaptureStdout();
  DbManager->printCollection("test");
  std::string output = testing::internal::GetCapturedStdout();

  EXPECT_NE(output.find("Resource E"), std::string::npos);
  EXPECT_NE(output.find("Resource F"), std::string::npos);
}

TEST_F(DataBaseTest, GetResourcesTest) {
  DbManager->insertResource("Resources",
                            {{"type", "Food"}, {"Name", "Resource G"}});
  DbManager->insertResource("Resources",
                            {{"type", "Food"}, {"Name", "Resource H"}});
  DbManager->insertResource("Resources",
                            {{"type", "Shelter"}, {"Name", "Resource I"}});

  auto resources = DbManager->getResources("Food");
  auto doc = bsoncxx::to_json(resources.view());

  EXPECT_NE(doc.find("Resource G"), std::string::npos);
  EXPECT_NE(doc.find("Resource H"), std::string::npos);
  EXPECT_EQ(doc.find("Resource I"), std::string::npos);
}
