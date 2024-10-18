// Copyright 2024 COMSW4156-Git-Gud

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include "MockDatabaseManager.h"
#include "Outreach.h"

/**
 * @brief Unit tests for the OutreachService class.
 *
 * This test fixture sets up a mock database manager and an
 * OutreachService instance for testing outreach-related functionalities.
 */
class OutreachServiceUnitTests : public ::testing::Test {
 protected:
  MockDatabaseManager* mockDbManager;
  Outreach* outreachService;

  void SetUp() override {
    mockDbManager = new MockDatabaseManager();
    outreachService = new Outreach(*mockDbManager, "Outreach");
  }

  void TearDown() override {
    delete outreachService;
    delete mockDbManager;
  }
};

/**
 * @brief Tests the getAllOutreachServices method.
 *
 * This test verifies that the getAllOutreachServices method returns
 * the expected outreach services from the mock database.
 */
TEST_F(OutreachServiceUnitTests, GetAllOutreachServices) {
  std::vector<bsoncxx::document::value> mockResult;
  mockResult.push_back(
      bsoncxx::builder::stream::document{}
      << "Target Audience" << "HML"
      << "Program Name" << "Emergency Shelter Access"
      << "Description"
      << "Provide information and assistance for accessing shelters."
      << "Program Date" << "05/01/24 - 12/31/24"
      << "Location" << "Bowery Mission, 227 Bowery, NY"
      << "Contact Info" << "Sarah Johnson, sarah@email.com"
      << bsoncxx::builder::stream::finalize);

  ON_CALL(*mockDbManager,
          findCollection(::testing::_, ::testing::_, ::testing::_))
      .WillByDefault(::testing::DoAll(::testing::SetArgReferee<2>(mockResult),
                                      ::testing::Return()));

  std::string services = outreachService->getAllOutreachServices();

  EXPECT_NE(services, "[]");
  EXPECT_TRUE(services.find("Emergency Shelter Access") != std::string::npos);
  EXPECT_TRUE(services.find("Bowery Mission, 227 Bowery, N") !=
              std::string::npos);
}

/**
 * @brief Tests the addOutreachService method.
 *
 * This test checks that the addOutreachService method correctly
 * inserts a new outreach service into the mock database and verifies
 * the expected parameters.
 */
TEST_F(OutreachServiceUnitTests, AddNewOutreachService) {
  std::vector<std::pair<std::string, std::string>> expectedContent = {
      {"targetAudience", "HML"},
      {"programName", "Emergency Shelter Access"},
      {"description",
       "Provide information and assistance for accessing shelters."},
      {"programDate", "05/01/24 - 12/31/24"},
      {"location", "Bowery Mission, 227 Bowery, NY"},
      {"contactInfo", "Sarah Johnson, sarah@email.com"}};

  ON_CALL(*mockDbManager, insertResource(::testing::_, ::testing::_))
      .WillByDefault(::testing::Invoke(
          [&](const std::string& collectionName,
              const std::vector<std::pair<std::string, std::string>>& content) {
            EXPECT_EQ(collectionName, "Outreach");
            EXPECT_EQ(content, expectedContent);
          }));

  std::string result = outreachService->addOutreachService(
      "HML", "Emergency Shelter Access",
      "Provide information and assistance for accessing shelters.",
      "05/01/24 - 12/31/24", "Bowery Mission, 227 Bowery, NY",
      "Sarah Johnson, sarah@email.com");

  EXPECT_EQ(result, "Success");
}
