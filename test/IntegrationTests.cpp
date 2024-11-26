#include <gtest/gtest.h>
#include <bsoncxx/json.hpp>
#include "DatabaseManager.h"
#include "RouteController.h"

class IntegrationTest : public ::testing::Test {
 protected:
  DatabaseManager* dbManager;
  RouteController* routeController;

  Shelter* shelter;
  Counseling* counseling;
  Food* food;
  Outreach* outreach;
  Healthcare* healthcare;
  AuthService* authService;
  SubscriptionManager* subscriptionManager;

  void SetUp() override {
    dbManager = new DatabaseManager("mongodb://localhost:27017");

    dbManager->createCollection("FoodTests");
    dbManager->createCollection("HealthcareTests");
    dbManager->createCollection("OutreachTests");
    dbManager->createCollection("ShelterTests");
    dbManager->createCollection("CounselingTests");
    dbManager->createCollection("UsersTests");
    dbManager->createCollection("SubscribersTests");

    shelter = new Shelter(*dbManager, "ShelterTests");
    counseling = new Counseling(*dbManager);
    food = new Food(*dbManager);
    outreach = new Outreach(*dbManager, "OutreachTests");
    healthcare = new Healthcare(*dbManager, "HealthcareTests");
    authService = new AuthService(*dbManager);
    subscriptionManager = new SubscriptionManager(*dbManager);

    routeController = new RouteController(
        *dbManager, *shelter, *counseling, *healthcare, *outreach, *food,
        *authService, *subscriptionManager);
  }

  void TearDown() override {
    dbManager->deleteCollection("FoodTests");
    dbManager->deleteCollection("HealthcareTests");
    dbManager->deleteCollection("OutreachTests");
    dbManager->deleteCollection("ShelterTests");
    dbManager->deleteCollection("CounselingTests");
    dbManager->deleteCollection("UsersTests");
    dbManager->deleteCollection("SubscribersTests");

    delete routeController;
    delete shelter;
    delete counseling;
    delete food;
    delete outreach;
    delete healthcare;
    delete authService;
    delete subscriptionManager;

    delete dbManager;
  }
};

TEST_F(IntegrationTest, TestAddHealthcareService) {
  std::string body =
      R"({
  "Name": "My New Hospital",
  "City" : "New York",
  "Address": "456 New St",
  "Description": "General Checkup",
  "HoursOfOperation": "9 AM - 5 PM",
  "eligibilityCriteria": "Adults",
  "ContactInfo": "123-456-7890"
})";

  crow::request req;
  req.add_header("Authorization",
                 "Bearer "
                 "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9."
                 "eyJlbWFpbCI6ImFkbWluQGdtYWlsLmNvbSIsImV4cCI6MjU5NjE0OTI0OCwia"
                 "WF0IjoxNzMyMTQ5MjQ4LCJpc3MiOiJhdXRoLXNlcnZpY2UiLCJyb2xlIjoidX"
                 "NlciIsInVzZXJJZCI6IjY3M2U4MDAwZDM1YTZiNGEzYzAwNTU5MiJ9."
                 "2TlZ1tnhclP708JotgxCLls0ekXX_Dmq9t5noG_xlOE");
  req.body = body;
  crow::response res{};

  routeController->addHealthcareService(req, res);

  EXPECT_EQ(res.code, 201);

  std::vector<bsoncxx::document::value> results;
  dbManager->findCollection(0, "HealthcareTests", {}, results);
  ASSERT_EQ(results.size(), 1);

  auto doc = bsoncxx::to_json(results[0].view());
  EXPECT_NE(doc.find("My New Hospital"), std::string::npos);
  EXPECT_NE(doc.find("New York"), std::string::npos);
  EXPECT_NE(doc.find("123-456-7890"), std::string::npos);
}

TEST_F(IntegrationTest, TestUpdateHealthcareService) {
  std::string initialBody =
      R"({
  "Name": "Old Hospital",
  "City" : "Boston",
  "Address": "123 Old St",
  "Description": "Basic Care",
  "HoursOfOperation": "9 AM - 5 PM",
  "eligibilityCriteria": "All Ages",
  "ContactInfo": "987-654-3210"
})";

  std::string id = healthcare->addHealthcareService(initialBody);

  std::string updateBody = R"({
  "id": ")" + id + R"(",
  "Name": "Updated Hospital",
  "City" : "New York",
  "Address": "456 New St",
  "Description": "Advanced Care",
  "HoursOfOperation": "9 AM - 9 PM",
  "eligibilityCriteria": "Adults",
  "ContactInfo": "123-456-7890"
})";

  crow::request req;
  req.add_header("Authorization",
                 "Bearer "
                 "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9."
                 "eyJlbWFpbCI6ImFkbWluQGdtYWlsLmNvbSIsImV4cCI6MjU5NjE0OTI0OCwia"
                 "WF0IjoxNzMyMTQ5MjQ4LCJpc3MiOiJhdXRoLXNlcnZpY2UiLCJyb2xlIjoidX"
                 "NlciIsInVzZXJJZCI6IjY3M2U4MDAwZDM1YTZiNGEzYzAwNTU5MiJ9."
                 "2TlZ1tnhclP708JotgxCLls0ekXX_Dmq9t5noG_xlOE");
  req.body = updateBody;
  crow::response res{};

  routeController->updateHealthcareService(req, res);

  EXPECT_EQ(res.code, 200);

  std::vector<bsoncxx::document::value> results;
  dbManager->findCollection(0, "HealthcareTests", {}, results);
  ASSERT_EQ(results.size(), 1);

  auto doc = bsoncxx::to_json(results[0].view());
  EXPECT_NE(doc.find("Updated Hospital"), std::string::npos);
  EXPECT_NE(doc.find("New York"), std::string::npos);
  EXPECT_NE(doc.find("123-456-7890"), std::string::npos);
}

TEST_F(IntegrationTest, TestDeleteHealthcareService) {
  std::string body =
      R"({
  "Name": "Hospital to Delete",
  "City" : "New York",
  "Address": "123 Delete St",
  "Description": "Temporary Service",
  "HoursOfOperation": "9 AM - 5 PM",
  "eligibilityCriteria": "All Ages",
  "ContactInfo": "123-123-1234"
})";

  std::string id = healthcare->addHealthcareService(body);

  std::string deleteBody = R"({"id": ")" + id + R"("})";

  crow::request req;
  req.add_header("Authorization",
                 "Bearer "
                 "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9."
                 "eyJlbWFpbCI6ImFkbWluQGdtYWlsLmNvbSIsImV4cCI6MjU5NjE0OTI0OCwia"
                 "WF0IjoxNzMyMTQ5MjQ4LCJpc3MiOiJhdXRoLXNlcnZpY2UiLCJyb2xlIjoidX"
                 "NlciIsInVzZXJJZCI6IjY3M2U4MDAwZDM1YTZiNGEzYzAwNTU5MiJ9."
                 "2TlZ1tnhclP708JotgxCLls0ekXX_Dmq9t5noG_xlOE");
  req.body = deleteBody;
  crow::response res{};

  routeController->deleteHealthcareService(req, res);

  EXPECT_EQ(res.code, 200);

  std::vector<bsoncxx::document::value> results;
  dbManager->findCollection(0, "HealthcareTests", {}, results);
  EXPECT_EQ(results.size(), 0);
}

TEST_F(IntegrationTest, TestGetAllHealthcareServices) {
  std::string body1 =
      R"({
  "Name": "Hospital A",
  "City" : "Boston",
  "Address": "123 Main St",
  "Description": "Primary Care",
  "HoursOfOperation": "8 AM - 4 PM",
  "eligibilityCriteria": "Adults",
  "ContactInfo": "111-111-1111"
})";

  std::string body2 =
      R"({
  "Name": "Hospital B",
  "City" : "New York",
  "Address": "456 Park Ave",
  "Description": "Specialist Care",
  "HoursOfOperation": "9 AM - 6 PM",
  "eligibilityCriteria": "All Ages",
  "ContactInfo": "222-222-2222"
})";

  healthcare->addHealthcareService(body1);
  healthcare->addHealthcareService(body2);

  crow::request req;
  req.add_header("Authorization",
                 "Bearer "
                 "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9."
                 "eyJlbWFpbCI6ImFkbWluQGdtYWlsLmNvbSIsImV4cCI6MjU5NjE0OTI0OCwia"
                 "WF0IjoxNzMyMTQ5MjQ4LCJpc3MiOiJhdXRoLXNlcnZpY2UiLCJyb2xlIjoidX"
                 "NlciIsInVzZXJJZCI6IjY3M2U4MDAwZDM1YTZiNGEzYzAwNTU5MiJ9."
                 "2TlZ1tnhclP708JotgxCLls0ekXX_Dmq9t5noG_xlOE");
  crow::response res{};

  routeController->getAllHealthcareServices(req, res);

  EXPECT_EQ(res.code, 200);

  auto resBody = res.body;
  EXPECT_NE(resBody.find("Hospital A"), std::string::npos);
  EXPECT_NE(resBody.find("Hospital B"), std::string::npos);
  EXPECT_NE(resBody.find("Boston"), std::string::npos);
  EXPECT_NE(resBody.find("New York"), std::string::npos);
  EXPECT_NE(resBody.find("111-111-1111"), std::string::npos);
  EXPECT_NE(resBody.find("222-222-2222"), std::string::npos);
}

