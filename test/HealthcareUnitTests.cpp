// Copyright 2024 COMSW4156-Git-Gud

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include "Healthcare.h"
#include "MockDatabaseManager.h"

class HealthcareServiceUnitTests : public ::testing::Test {
 protected:
  MockDatabaseManager* mockDbManager;
  Healthcare* healthcareService;

  void SetUp() override {
    mockDbManager = new MockDatabaseManager();
    healthcareService = new Healthcare(*mockDbManager, "HealthcareTest");
  }

  void TearDown() override {
    delete healthcareService;
    delete mockDbManager;
  }
};

TEST_F(HealthcareServiceUnitTests, GetAllHealthcareServices) {
  std::vector<bsoncxx::document::value> mockResult;
  mockResult.push_back(bsoncxx::builder::stream::document{}
                       << "name" << "City Hospital"
                       << "service" << "General Checkup"
                       << "address" << "123 Main St"
                       << "hours" << "9 AM - 5 PM"
                       << "audience" << "Adults"
                       << "phone" << "123-456-7890"
                       << bsoncxx::builder::stream::finalize);

  ON_CALL(*mockDbManager,
          findCollection(::testing::_, ::testing::_, ::testing::_))
      .WillByDefault(::testing::DoAll(::testing::SetArgReferee<2>(mockResult),
                                      ::testing::Return()));

  std::string services = healthcareService->getAllHealthcareServices();

  EXPECT_NE(services, "[]");
  EXPECT_TRUE(services.find("City Hospital") != std::string::npos);
  EXPECT_TRUE(services.find("General Checkup") != std::string::npos);
  EXPECT_TRUE(services.find("123 Main St") != std::string::npos);
}

TEST_F(HealthcareServiceUnitTests, AddNewHealthcareService) {
  std::map<std::string, std::string> updates = {
      {"provider", "City Hospital"},
      {"serviceType", "General Checkup"},
      {"location", "123 Main St"},
      {"operatingHours", "9 AM - 5 PM"},
      {"eligibilityCriteria", "Adults"},
      {"contactInfo", "123-456-7890"}
  };

  // for comparison in mock call
  std::vector<std::pair<std::string, std::string>> expectedContent = {
      {"provider", "City Hospital"},
      {"serviceType", "General Checkup"},
      {"location", "123 Main St"},
      {"operatingHours", "9 AM - 5 PM"},
      {"eligibilityCriteria", "Adults"},
      {"contactInfo", "123-456-7890"}
  };

  ON_CALL(*mockDbManager, insertResource(::testing::_, ::testing::_))
      .WillByDefault(::testing::Invoke(
          [&](const std::string& collectionName,
              const std::vector<std::pair<std::string, std::string>>& content) {
            EXPECT_EQ(collectionName, "HealthcareTest");
            EXPECT_EQ(content, expectedContent);
            return "12345";
          }));

  std::string result = healthcareService->addHealthcareService(updates);

  EXPECT_EQ(result, "12345");
}

TEST_F(HealthcareServiceUnitTests, DeleteHealthcare) {
  std::string mockId = "123";

  ON_CALL(*mockDbManager, deleteResource(::testing::_, ::testing::_))
      .WillByDefault([&](const std::string& collectionName,
                         const std::string& resourceId) -> bool {
        EXPECT_EQ(resourceId, mockId);
        EXPECT_EQ(collectionName, "HealthcareTest");
        return true;
      });

  std::string result = healthcareService->deleteHealthcare(mockId);
  EXPECT_EQ(result, "Healthcare record deleted successfully.");

  std::vector<bsoncxx::document::value> mockResult;
  ON_CALL(*mockDbManager, findCollection(::testing::_, ::testing::_, ::testing::_))
      .WillByDefault(::testing::DoAll(::testing::SetArgReferee<2>(mockResult),
                                      ::testing::Return()));

  std::string healthcareItems = healthcareService->getAllHealthcareServices();
  EXPECT_EQ(healthcareItems, "[]");
}

TEST_F(HealthcareServiceUnitTests, UpdateHealthcare) {
  std::string mockId = "123";
  std::vector<std::pair<std::string, std::string>> updateResource = {
      {"provider", "Updated Healthcare Provider"},
      {"serviceType", "Updated Emergency Care"},
      {"location", "789 Oak St"},
      {"operatingHours", "24/7"},
      {"contactInfo", "321-654-0987"}
  };

  ON_CALL(*mockDbManager, updateResource(::testing::_, ::testing::_, ::testing::_))
      .WillByDefault(
          [&](const std::string& collectionName, const std::string& resourceId,
              const std::vector<std::pair<std::string, std::string>>& content) -> bool {
            EXPECT_EQ(resourceId, mockId);
            EXPECT_EQ(collectionName, "HealthcareTest");
            EXPECT_EQ(content, updateResource);
            return true;
          });

  std::string result = healthcareService->updateHealthcare(mockId, {
      {"provider", "Updated Healthcare Provider"},
      {"serviceType", "Updated Emergency Care"},
      {"location", "789 Oak St"},
      {"operatingHours", "24/7"},
      {"contactInfo", "321-654-0987"}
  });
  EXPECT_EQ(result, "Healthcare record updated successfully.");
  
  std::vector<bsoncxx::document::value> mockResult;
  mockResult.push_back(bsoncxx::builder::stream::document{}
                       << "provider" << "Updated Healthcare Provider"
                       << "serviceType" << "Updated Emergency Care"
                       << "location" << "789 Oak St"
                       << "operatingHours" << "24/7"
                       << "contactInfo" << "321-654-0987"
                       << bsoncxx::builder::stream::finalize);

  ON_CALL(*mockDbManager, findCollection(::testing::_, ::testing::_, ::testing::_))
      .WillByDefault(::testing::DoAll(::testing::SetArgReferee<2>(mockResult),
                                      ::testing::Return()));

  std::string healthcareItems = healthcareService->getAllHealthcareServices();
  EXPECT_TRUE(healthcareItems.find("Updated Healthcare Provider") != std::string::npos);
  EXPECT_TRUE(healthcareItems.find("Updated Emergency Care") != std::string::npos);
  EXPECT_TRUE(healthcareItems.find("789 Oak St") != std::string::npos);
  EXPECT_TRUE(healthcareItems.find("24/7") != std::string::npos);
  EXPECT_TRUE(healthcareItems.find("321-654-0987") != std::string::npos);
}

