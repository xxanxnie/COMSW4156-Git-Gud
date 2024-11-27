// Copyright 2024 COMSW4156-Git-Gud

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include "DatabaseManager.h"

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
