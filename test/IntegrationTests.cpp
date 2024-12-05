// Copyright 2024 COMSW4156-Git-Gud
#include <gtest/gtest.h>

#include <bsoncxx/json.hpp>

#include "DatabaseManager.h"
#include "RouteController.h"

// For GET endpoints (getall), use HML token:
inline std::string getValidTokenForGet() {
  return "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9."
         "eyJlbWFpbCI6ImFkYWFAZ21haWwuY29tIiwiZXhwIjoyNTk2NjgwMDI3LCJpYXQiOjE3M"
         "z"
         "I2ODAwMjcsImlzcyI6ImF1dGgtc2VydmljZSIsInJvbGUiOiJITUwiLCJ1c2VySWQiOiI"
         "2"
         "NzQ2OTk1YjFiZmFiODQ2NDEwNjZjNjMifQ."
         "N0l6jhy5WfHEQCqq82OMPsoSPFobNMlyEHQ0M3Qo87A";
}

// For POST endpoints, use NGO token:
inline std::string getValidTokenForPost() {
  return "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9."
         "eyJlbWFpbCI6ImFkYUBnbWFpbC5jb20iLCJleHAiOjI1OTY2Nzk5OTAsImlhdCI6MTczM"
         "j"
         "Y3OTk5MCwiaXNzIjoiYXV0aC1zZXJ2aWNlIiwicm9sZSI6Ik5HTyIsInVzZXJJZCI6IjY"
         "3"
         "NDY5OTM2MWJmYWI4NDY0MTA2NmM2MiJ9.HrxegAGsSbQqX8h1m3F-o8fkuf4-"
         "j2q6qgA7pOYolwc";
}

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
    counseling = new Counseling(*dbManager, "CounselingTests");
    food = new Food(*dbManager, "FoodTests");
    outreach = new Outreach(*dbManager, "OutreachTests");
    healthcare = new Healthcare(*dbManager, "HealthcareTests");
    authService = new AuthService(*dbManager);
    subscriptionManager = new SubscriptionManager(*dbManager);

    routeController = new RouteController(*dbManager, *shelter, *counseling,
                                          *healthcare, *outreach, *food,
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
  req.add_header("Authorization", "Bearer " + getValidTokenForPost());
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

  std::string id = healthcare->addHealthcareService(initialBody, "456");

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
  req.add_header("Authorization", "Bearer " + getValidTokenForPost());
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

  std::string id = healthcare->addHealthcareService(
      body, "Bearer " + getValidTokenForPost());

  std::string deleteBody = R"({"id": ")" + id + R"("})";

  crow::request req;
  req.add_header("Authorization", "Bearer " + getValidTokenForPost());
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

  healthcare->addHealthcareService(body1, "456");
  healthcare->addHealthcareService(body2, "456");

  crow::request req;
  req.add_header("Authorization", "Bearer " + getValidTokenForGet());
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

TEST_F(IntegrationTest, TestGetAllOutreachServices) {
  dbManager->insertResource(
      "OutreachTests",
      {{"Name", "Emergency Shelter Access"},
       {"City", "New York"},
       {"Address", "200 Varick St, New York, NY 10014"},
       {"Description",
        "Provide information and assistance for accessing shelters."},
       {"ContactInfo", "Sarah Johnson, sarah@email.com"},
       {"HoursOfOperation", "05/01/24 - 12/31/24"},
       {"TargetAudience", "HML"}});

  crow::request req{};
  req.add_header("Authorization", "Bearer " + getValidTokenForGet());
  crow::response res{};

  routeController->getAllOutreachServices(req, res);

  EXPECT_EQ(res.code, 200);

  auto resBody = res.body;
  EXPECT_NE(resBody.find("Emergency Shelter Access"), std::string::npos);
  EXPECT_NE(resBody.find("200 Varick St, New York, NY 10014"),
            std::string::npos);
}

TEST_F(IntegrationTest, TestAddOutreachServiceTest) {
  std::string body =
      R"({
    "Name": "Emergency Shelter Access",
    "City": "New York",
    "Address": "200 Varick St, New York, NY 10014",
    "Description": "Provide information and assistance for accessing shelters.",
    "ContactInfo": "Sarah Johnson, sarah@email.com",
    "HoursOfOperation": "05/01/24 - 12/31/24",
    "TargetAudience": "HML"
})";

  crow::request req;
  req.add_header("Authorization", "Bearer " + getValidTokenForPost());
  req.body = body;
  crow::response res{};

  routeController->addOutreachService(req, res);

  EXPECT_EQ(res.code, 201);

  std::vector<bsoncxx::document::value> results;
  dbManager->findCollection(0, "OutreachTests", {}, results);
  ASSERT_EQ(results.size(), 1);

  auto doc = bsoncxx::to_json(results[0].view());
  EXPECT_NE(doc.find("Emergency Shelter Access"), std::string::npos);
  EXPECT_NE(doc.find("New York"), std::string::npos);
  EXPECT_NE(doc.find("Sarah Johnson, sarah@email.com"), std::string::npos);
}

TEST_F(IntegrationTest, TestGetAllShelterTest) {
  dbManager->insertResource(
      "ShelterTests",
      {{"Name", "Shelter For All"},
       {"City", "New York"},
       {"Address", "200 New St, New York, NY 10014"},
       {"Description",
        "A safe space providing temporary shelter forindividuals in need."},
       {"ContactInfo", "Sarah Johnson, sarah@email.com"},
       {"HoursOfOperation", "05/01/24 - 12/31/24"},
       {"ORG", "NGO"},
       {"TargetUser", "homeless"},
       {"Capacity", "50"},
       {"CurrentUse", "10"}});

  crow::request req{};
  req.add_header("Authorization", "Bearer " + getValidTokenForGet());
  crow::response res{};

  routeController->getShelter(req, res);

  EXPECT_EQ(res.code, 200);

  auto resBody = res.body;
  EXPECT_NE(resBody.find("Shelter For All"), std::string::npos);
  EXPECT_NE(resBody.find("200 New St, New York, NY 10014"), std::string::npos);
}

TEST_F(IntegrationTest, AddShelterTest) {
  std::string body =
      R"({
    "Name": "temp",
    "City": "New York",
    "Address": "temp",
    "Description": "NULL",
    "ContactInfo": "66664566565",
    "HoursOfOperation": "2024-01-11",
    "ORG": "NGO",
    "TargetUser": "homeless",
    "Capacity": "100",
    "CurrentUse": "10"
})";

  crow::request req;
  req.add_header("Authorization", "Bearer " + getValidTokenForPost());
  req.body = body;
  crow::response res{};

  routeController->addShelter(req, res);

  EXPECT_EQ(res.code, 201);

  std::vector<bsoncxx::document::value> results;
  dbManager->findCollection(0, "ShelterTests", {}, results);
  ASSERT_EQ(results.size(), 1);

  auto doc = bsoncxx::to_json(results[0].view());
  EXPECT_NE(doc.find("temp"), std::string::npos);
  EXPECT_NE(doc.find("New York"), std::string::npos);
  EXPECT_NE(doc.find("66664566565"), std::string::npos);
}
