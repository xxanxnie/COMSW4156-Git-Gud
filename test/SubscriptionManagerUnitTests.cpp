// Copyright 2024 COMSW4156-Git-Gud

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include "MockDatabaseManager.h"
#include "SubscriptionManager.h"

class SubscriptionManagerUnitTests : public ::testing::Test {
 protected:
  MockDatabaseManager* mockDbManager;
  SubscriptionManager* subscriptionManager;

  void SetUp() override {
    mockDbManager = new MockDatabaseManager();
    subscriptionManager = new SubscriptionManager(*mockDbManager);
  }

  void TearDown() override {
    delete subscriptionManager;
    delete mockDbManager;
  }
};

TEST_F(SubscriptionManagerUnitTests, AddSubscriber) {
  std::map<std::string, std::string> subscriberDetails = {
      {"Resource", "Healthcare"},
      {"City", "New York"},
      {"Contact", "user@example.com"}};

  std::vector<std::pair<std::string, std::string>> expectedKeyValues = {
      {"Resource", "Healthcare"},
      {"City", "New York"},
      {"Contact", "user@example.com"}};

  ON_CALL(*mockDbManager,
          insertResource("Subscribers", ::testing::UnorderedElementsAreArray(
                                            expectedKeyValues)))
      .WillByDefault(::testing::Return("mock_id_12345"));

  std::string result = subscriptionManager->addSubscriber(subscriberDetails);

  EXPECT_EQ(result, "mock_id_12345");
}

TEST_F(SubscriptionManagerUnitTests, DeleteSubscriber_Success) {
  std::string subscriberId = "12345";

  ON_CALL(*mockDbManager, deleteResource("Subscribers", subscriberId, ""))
      .WillByDefault(::testing::Return(true));

  std::string result = subscriptionManager->deleteSubscriber(subscriberId);

  EXPECT_EQ(result, "Subscriber deleted successfully.");
}

TEST_F(SubscriptionManagerUnitTests, DeleteSubscriber_NotFound) {
  std::string subscriberId = "12345";

  ON_CALL(*mockDbManager, deleteResource("Subscribers", subscriberId, ""))
      .WillByDefault(::testing::Return(false));

  std::string result = subscriptionManager->deleteSubscriber(subscriberId);

  EXPECT_EQ(result, "Error: Subscriber not found.");
}

TEST_F(SubscriptionManagerUnitTests, GetSubscribers) {
  std::string resource = "Healthcare";
  std::string city = "New York";

  std::vector<std::pair<std::string, std::string>> expectedQuery = {
      {"Resource", resource}, {"City", city}};

  std::vector<bsoncxx::document::value> mockDocs;

  bsoncxx::builder::stream::document doc1;
  doc1 << "_id" << bsoncxx::oid("507f1f77bcf86cd799439011") << "Contact"
       << "user1@example.com";
  mockDocs.push_back(doc1.extract());

  bsoncxx::builder::stream::document doc2;
  doc2 << "_id" << bsoncxx::oid("507f1f77bcf86cd799439012") << "Contact"
       << "user2@example.com";
  mockDocs.push_back(doc2.extract());

  ON_CALL(*mockDbManager,
          findCollection(0, "Subscribers", expectedQuery, ::testing::_))
      .WillByDefault(::testing::DoAll(::testing::SetArgReferee<3>(mockDocs),
                                      ::testing::Return()));

  std::map<std::string, std::string> subscribers =
      subscriptionManager->getSubscribers(resource, city);

  EXPECT_EQ(subscribers.size(), 2);
  EXPECT_EQ(subscribers["507f1f77bcf86cd799439011"], "user1@example.com");
  EXPECT_EQ(subscribers["507f1f77bcf86cd799439012"], "user2@example.com");
}
