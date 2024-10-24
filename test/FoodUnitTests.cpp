// Copyright 2024 COMSW4156-Git-Gud

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include "Food.h"
#include "MockDatabaseManager.h"

class FoodUnitTests : public ::testing::Test {
 protected:
  Food* food;
  MockDatabaseManager* mockDbManager;

  void SetUp() override {
    mockDbManager = new MockDatabaseManager();
    food = new Food(*mockDbManager);
  }

  void TearDown() override {
    delete food;
    delete mockDbManager;
  }
};

TEST_F(FoodUnitTests, getAllFood) {
  std::vector<bsoncxx::document::value> mockResult;
  mockResult.push_back(bsoncxx::builder::stream::document{}
                       << "FoodType" << "Fruits"
                       << "Provider" << "LocalFarm"
                       << "location" << "Brooklyn"
                       << "quantity" << "100"
                       << "expirationDate" << "2024-12-31"
                       << bsoncxx::builder::stream::finalize);

  ON_CALL(*mockDbManager,
          findCollection(::testing::_, ::testing::_, ::testing::_))
      .WillByDefault(::testing::DoAll(::testing::SetArgReferee<2>(mockResult),
                                      ::testing::Return()));

  std::string foodItems = food->getAllFood();
  EXPECT_NE(foodItems, "[]");
  EXPECT_TRUE(foodItems.find("Fruits") != std::string::npos);
  EXPECT_TRUE(foodItems.find("LocalFarm") != std::string::npos);
  EXPECT_TRUE(foodItems.find("Brooklyn") != std::string::npos);
}

TEST_F(FoodUnitTests, addFood) {
  std::vector<std::pair<std::string, std::string>> foodResource = {
      {"FoodType", "Vegetables"},
      {"Provider", "OrganicFarm"},
      {"location", "Queens"},
      {"quantity", "50"},
      {"expirationDate", "2024-11-30"}};

  ON_CALL(*mockDbManager, insertResource("Food", foodResource))
      .WillByDefault(::testing::Return());

  food->addFood(foodResource);

  std::vector<bsoncxx::document::value> mockResult;
  mockResult.push_back(bsoncxx::builder::stream::document{}
                       << "FoodType" << "Vegetables"
                       << "Provider" << "OrganicFarm"
                       << "location" << "Queens"
                       << "quantity" << "50"
                       << "expirationDate" << "2024-11-30"
                       << bsoncxx::builder::stream::finalize);

  ON_CALL(*mockDbManager,
          findCollection(::testing::_, ::testing::_, ::testing::_))
      .WillByDefault(::testing::DoAll(::testing::SetArgReferee<2>(mockResult),
                                      ::testing::Return()));

  std::string foodItems = food->getAllFood();

  EXPECT_NE(foodItems, "[]");
  EXPECT_TRUE(foodItems.find("Vegetables") != std::string::npos);
  EXPECT_TRUE(foodItems.find("OrganicFarm") != std::string::npos);
  EXPECT_TRUE(foodItems.find("Queens") != std::string::npos);
  EXPECT_TRUE(foodItems.find("50") != std::string::npos);
  EXPECT_TRUE(foodItems.find("2024-11-30") != std::string::npos);
}
