// Copyright 2024 COMSW4156-Git-Gud

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Counseling.h"
#include "Food.h"
#include "Healthcare.h"
#include "MockDatabaseManager.h"
#include "Outreach.h"
#include "RouteController.h"
#include "Shelter.h"

class MockShelter : public Shelter {
 public:
  explicit MockShelter(DatabaseManager* dbManager)
      : Shelter(*dbManager, "ShelterTest") {}

  MOCK_METHOD(std::string, addShelter, (std::string request_body), (override));
  MOCK_METHOD(std::string, deleteShelter, (std::string id), (override));
  MOCK_METHOD(std::string, searchShelterAll, (int start), (override));
  MOCK_METHOD(std::string, updateShelter, (std::string request_body),
              (override));
};

class MockCounseling : public Counseling {
 public:
  explicit MockCounseling(DatabaseManager* dbManager,
                          const std::string& collection_name)
      : Counseling(*dbManager, "CounselingTests") {}

  MOCK_METHOD(std::string, addCounselor, (std::string request_body),
              (override));
  MOCK_METHOD(std::string, deleteCounselor, (const std::string& counselorId),
              (override));
  MOCK_METHOD(std::string, searchCounselorsAll, (int start), (override));
  MOCK_METHOD(std::string, updateCounselor, (std::string request_body),
              (override));
};

class MockFood : public Food {
 public:
  explicit MockFood(DatabaseManager* db, const std::string& collection_name)
      : Food(*db, "FoodTests") {}

  MOCK_METHOD(std::string, addFood, ((std::string request_body)), (override));
  MOCK_METHOD(std::string, getAllFood, (int start), (override));
  MOCK_METHOD(std::string, deleteFood, (const std::string& counselorId),
              (override));
  MOCK_METHOD(std::string, updateFood, (std::string request_body), (override));
};

class MockOutreachService : public Outreach {
 public:
  MockOutreachService(DatabaseManager* dbManager,
                      const std::string& collection_name)
      : Outreach(*dbManager, collection_name) {}

  MOCK_METHOD(std::string, addOutreachService, (std::string request_body),
              (override));
  MOCK_METHOD(std::string, getAllOutreachServices, (int start), (override));
  MOCK_METHOD(std::string, deleteOutreach, (std::string counselorId),
              (override));
  MOCK_METHOD(std::string, updateOutreach, (std::string request_body),
              (override));
};

class MockHealthcareService : public Healthcare {
 public:
  MockHealthcareService(DatabaseManager* dbManager,
                        const std::string& collection_name)
      : Healthcare(*dbManager, collection_name) {}

  MOCK_METHOD(std::string, addHealthcareService, ((std::string request_body)),
              (override));
  MOCK_METHOD(std::string, getAllHealthcareServices, (int start), (override));

  MOCK_METHOD(std::string, updateHealthcare, (std::string request_body),
              (override));

  MOCK_METHOD(std::string, deleteHealthcare, ((std::string)), (override));
};

class MockAuthService : public AuthService {
 public:
  explicit MockAuthService(DatabaseManager& dbManager) : AuthService(dbManager) {}
};

class MockSubscriptionManager : public SubscriptionManager {
 public:
  explicit MockSubscriptionManager(DatabaseManager* dbManager)
      : SubscriptionManager(*dbManager) {}

  MOCK_METHOD(std::string, addSubscriber,
              ((const std::map<std::string, std::string>& subscriberDetails)),
              (override));
  MOCK_METHOD(std::string, deleteSubscriber, (const std::string& id),
              (override));
  MOCK_METHOD((std::map<std::string, std::string>), getSubscribers,
              (const std::string&, (const std::string&)), (override));
  MOCK_METHOD(void, notifySubscribers,
              ((const std::string&), (const std::string&)), (override));
};

class RouteControllerUnitTests : public ::testing::Test {
 protected:
  MockDatabaseManager* mockDbManager;
  MockShelter* mockShelter;
  MockCounseling* mockCounseling;
  MockFood* mockFood;
  MockOutreachService* mockOutreach;
  MockHealthcareService* mockHealthcare;
  MockAuthService* mockAuthService;
  MockSubscriptionManager* mockSubscriptionManager;
  RouteController* routeController;

  void SetUp() override {
    mockDbManager = new MockDatabaseManager();
    mockShelter = new MockShelter(mockDbManager);
    mockCounseling = new MockCounseling(mockDbManager, "CounselingServiceTest");
    mockFood = new MockFood(mockDbManager, "FoodServiceTest");
    mockOutreach =
        new MockOutreachService(mockDbManager, "OutreachServiceTest");
    mockHealthcare =
        new MockHealthcareService(mockDbManager, "HealthcareServiceTest");
    mockAuthService = new MockAuthService(*mockDbManager);

    mockSubscriptionManager = new MockSubscriptionManager(mockDbManager);
    routeController = new RouteController(
        *mockDbManager, *mockShelter, *mockCounseling, *mockHealthcare,
        *mockOutreach, *mockFood, *mockAuthService, *mockSubscriptionManager);
  }

  void TearDown() override {
    delete routeController;
    delete mockHealthcare;
    delete mockOutreach;
    delete mockFood;
    delete mockCounseling;
    delete mockShelter;
    delete mockDbManager;
    delete mockSubscriptionManager;
  }
};

// For GET endpoints (getall), use HML token:
std::string getValidTokenForGet() {
  return "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9."
         "eyJlbWFpbCI6ImFkYWFAZ21haWwuY29tIiwiZXhwIjoyNTk2NjgwMDI3LCJpYXQiOjE3Mz"
         "I2ODAwMjcsImlzcyI6ImF1dGgtc2VydmljZSIsInJvbGUiOiJITUwiLCJ1c2VySWQiOiI2"
         "NzQ2OTk1YjFiZmFiODQ2NDEwNjZjNjMifQ."
         "N0l6jhy5WfHEQCqq82OMPsoSPFobNMlyEHQ0M3Qo87A";
}

// For POST endpoints, use NGO token:
std::string getValidTokenForPost() {
  return "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9."
         "eyJlbWFpbCI6ImFkYUBnbWFpbC5jb20iLCJleHAiOjI1OTY2Nzk5OTAsImlhdCI6MTczMj"
         "Y3OTk5MCwiaXNzIjoiYXV0aC1zZXJ2aWNlIiwicm9sZSI6Ik5HTyIsInVzZXJJZCI6IjY3"
         "NDY5OTM2MWJmYWI4NDY0MTA2NmM2MiJ9.HrxegAGsSbQqX8h1m3F-o8fkuf4-"
         "j2q6qgA7pOYolwc";
}

TEST_F(RouteControllerUnitTests, GetShelterTestAuthorized) {
  std::string mockResponse =
      R"([{"ORG": "NGO", "User": "HML", "location": "NYC"}])";
  ON_CALL(*mockShelter, searchShelterAll(0))
      .WillByDefault(::testing::Return(mockResponse));

  crow::request req{};
  req.add_header("Authorization", "Bearer " + getValidTokenForGet());
  crow::response res{};

  routeController->getShelter(req, res);

  EXPECT_EQ(res.code, 200);
  EXPECT_EQ(res.body, mockResponse);
}

TEST_F(RouteControllerUnitTests, AddShelterTestAuthorized) {
  std::string body =
      "{\"Name\" : \"temp\",\"City\" : \"New York\",\"Address\": "
      "\"temp\",\"Description\" : \"NULL\",\"ContactInfo\" : "
      "\"66664566565\",\"HoursOfOperation\": "
      "\"2024-01-11\",\"ORG\":\"NGO\",\"TargetUser\" "
      ":\"homeless\",\"Capacity\" : \"100\",\"CurrentUse\": \"10\"}";
  crow::request req;
  req.body = body;
  req.add_header("Authorization", "Bearer " + getValidTokenForPost());
  crow::response res{};

  ON_CALL(*mockShelter,
          addShelter(
              "{\"Name\" : \"temp\",\"City\" : \"New York\",\"Address\": "
              "\"temp\",\"Description\" : \"NULL\",\"ContactInfo\" : "
              "\"66664566565\",\"HoursOfOperation\": "
              "\"2024-01-11\",\"ORG\":\"NGO\",\"TargetUser\" "
              ":\"homeless\",\"Capacity\" : \"100\",\"CurrentUse\": \"10\"}"))
      .WillByDefault(::testing::Return("Success"));

  routeController->addShelter(req, res);

  EXPECT_EQ(res.code, 201);
}

TEST_F(RouteControllerUnitTests, GetShelterTestUnauthorized) {
  std::string mockResponse =
      R"([{"ORG": "NGO", "User": "HML", "location": "NYC"}])";
  ON_CALL(*mockShelter, searchShelterAll(0))
      .WillByDefault(::testing::Return(mockResponse));

  crow::request req{};
  crow::response res{};

  routeController->getShelter(req, res);

  EXPECT_EQ(res.code, 401);
  EXPECT_EQ(res.body, "Authentication required. Please provide a valid token.");
}

TEST_F(RouteControllerUnitTests, AddShelterTestUnauthorized) {
  std::string body =
      "{\"Name\" : \"temp\",\"City\" : \"New York\",\"Address\": "
      "\"temp\",\"Description\" : \"NULL\",\"ContactInfo\" : "
      "\"66664566565\",\"HoursOfOperation\": "
      "\"2024-01-11\",\"ORG\":\"NGO\",\"TargetUser\" "
      ":\"homeless\",\"Capacity\" : \"100\",\"CurrentUse\": \"10\"}";
  crow::request req;
  req.body = body;
  req.add_header("Authorization", "Bearer invalid.token.here");
  crow::response res{};

  ON_CALL(*mockShelter,
          addShelter(
              "{\"Name\" : \"temp\",\"City\" : \"New York\",\"Address\": "
              "\"temp\",\"Description\" : \"NULL\",\"ContactInfo\" : "
              "\"66664566565\",\"HoursOfOperation\": "
              "\"2024-01-11\",\"ORG\":\"NGO\",\"TargetUser\" "
              ":\"homeless\",\"Capacity\" : \"100\",\"CurrentUse\": \"10\"}"))
      .WillByDefault(::testing::Return("Success"));

  routeController->addShelter(req, res);

  EXPECT_EQ(res.code, 401);
  EXPECT_EQ(res.body, "Invalid or expired token.");
}
TEST_F(RouteControllerUnitTests, DeleteShelterTestAuthorized) {
  std::string body = R"({"id": "507f191e810c19729de860ea"})";
  crow::request req;
  req.add_header("Authorization", "Bearer " + getValidTokenForPost());
  req.body = body;
  crow::response res{};

  std::string id = "507f191e810c19729de860ea";

  ON_CALL(*mockShelter, deleteShelter(id))
      .WillByDefault(
          ::testing::Return("Shelter resource deleted successfully."));

  routeController->deleteShelter(req, res);

  EXPECT_EQ(res.code, 200);
  EXPECT_EQ(res.body, "Shelter resource deleted successfully.");
}

TEST_F(RouteControllerUnitTests, DeleteShelterTestUnauthorized) {
  std::string body = R"({"id": "507f191e810c19729de860ea"})";
  crow::request req;
  req.add_header("Authorization", "Bearer invalid.token.here");
  req.body = body;
  crow::response res{};

  routeController->deleteShelter(req, res);

  EXPECT_EQ(res.code, 401);
  EXPECT_EQ(res.body, "Invalid or expired token.");
}
TEST_F(RouteControllerUnitTests, UpdateShelterTestAuthorized) {
  std::string body = "";
  crow::request req;
  req.add_header("Authorization", "Bearer " + getValidTokenForPost());
  req.body = body;
  crow::response res{};

  ON_CALL(*mockShelter, updateShelter(body))
      .WillByDefault(
          ::testing::Return("Shelter resource updated successfully."));

  routeController->updateShelter(req, res);

  EXPECT_EQ(res.code, 200);
  EXPECT_EQ(res.body, "Shelter resource updated successfully.");
}

TEST_F(RouteControllerUnitTests, GetCounselingTestAuthorized) {
  std::string mockResponse = R"([{"name": "John Doe", "specialty": "CBT"}])";
  ON_CALL(*mockCounseling, searchCounselorsAll(0))
      .WillByDefault(::testing::Return(mockResponse));

  crow::request req{};
  req.add_header("Authorization", "Bearer " + getValidTokenForGet());
  crow::response res{};
  routeController->getCounseling(req, res);

  EXPECT_EQ(res.code, 200);
  EXPECT_EQ(res.body, mockResponse);
}

TEST_F(RouteControllerUnitTests, AddCounselingTestAuthorized) {
  std::string body = R"({
        "Name" : "OrganicFarm",
        "City" : "New York",
        "Address": "temp",
        "Description" : "Vegetables",
        "ContactInfo" : "66664566565",
        "HoursOfOperation": "2024-01-11",
        "counselorName": "Jack"
    })";
  crow::request req;
  req.body = body;
  req.add_header("Authorization", "Bearer " + getValidTokenForPost());
  crow::response res{};

  ON_CALL(*mockCounseling, addCounselor(body))
      .WillByDefault(::testing::Return("Suc"));

  routeController->addCounseling(req, res);

  EXPECT_EQ(res.code, 201);
}

TEST_F(RouteControllerUnitTests, GetCounselingTestUnauthorized) {
  std::string mockResponse = R"([{"name": "John Doe", "specialty": "CBT"}])";
  ON_CALL(*mockCounseling, searchCounselorsAll(0))
      .WillByDefault(::testing::Return(mockResponse));

  crow::request req{};
  crow::response res{};

  routeController->getCounseling(req, res);

  EXPECT_EQ(res.code, 401);
  EXPECT_EQ(res.body, "Authentication required. Please provide a valid token.");
}

TEST_F(RouteControllerUnitTests, AddCounselingTestUnauthorized) {
  std::string body = R"({
        "Name" : "New York Psychological Association",
        "City" : "New York",
        "Address": "211 E 43rd St, New York, NY 10017",
        "Description" : "Provides mental health counseling and therapy services.",
        "ContactInfo" : "66664566565",
        "HoursOfOperation": "2024-01-11",
        "counselorName": "Jack"
    })";
  crow::request req;
  req.body = body;
  req.add_header("Authorization", "Bearer invalid.token.here");
  crow::response res{};

  ON_CALL(*mockCounseling, addCounselor(body))
      .WillByDefault(::testing::Return("Success"));

  routeController->addCounseling(req, res);

  EXPECT_EQ(res.code, 401);
  EXPECT_EQ(res.body, "Invalid or expired token.");
}
TEST_F(RouteControllerUnitTests, DeleteCounselingTestAuthorized) {
  std::string body = R"({"id": "507f191e810c19729de860ea"})";
  crow::request req;
  req.add_header("Authorization", "Bearer " + getValidTokenForPost());
  req.body = body;
  crow::response res{};

  std::string id = "507f191e810c19729de860ea";

  ON_CALL(*mockCounseling, deleteCounselor(id))
      .WillByDefault(
          ::testing::Return("Counseling resource deleted successfully."));

  routeController->deleteCounseling(req, res);

  EXPECT_EQ(res.code, 200);
  EXPECT_EQ(res.body, "Counseling resource deleted successfully.");
}

TEST_F(RouteControllerUnitTests, DeleteCounselingTestUnauthorized) {
  std::string body = R"({"id": "507f191e810c19729de860ea"})";
  crow::request req;
  req.add_header("Authorization", "Bearer invalid.token.here");
  req.body = body;
  crow::response res{};

  routeController->deleteCounseling(req, res);

  EXPECT_EQ(res.code, 401);
  EXPECT_EQ(res.body, "Invalid or expired token.");
}
TEST_F(RouteControllerUnitTests, UpdateCounselingTestAuthorized) {
  std::string body = "";
  crow::request req;
  req.add_header("Authorization", "Bearer " + getValidTokenForPost());
  req.body = body;
  crow::response res{};

  ON_CALL(*mockCounseling, updateCounselor(body))
      .WillByDefault(
          ::testing::Return("Counseling resource updated successfully."));

  routeController->updateCounseling(req, res);

  EXPECT_EQ(res.code, 200);
  EXPECT_EQ(res.body, "Counseling resource updated successfully.");
}
TEST_F(RouteControllerUnitTests, GetAllFoodTestAuthorized) {
  crow::request req;
  crow::response res;

  // Set valid API key for GET operations
  req.add_header("Authorization", "Bearer " + getValidTokenForGet());

  std::string mockResponse =
      "[{\"name\": \"FoodBank\", \"location\": \"NYC\"}]";
  ON_CALL(*mockFood, getAllFood(0))
      .WillByDefault(::testing::Return(mockResponse));

  routeController->getAllFood(req, res);

  EXPECT_EQ(res.code, 200);
  EXPECT_EQ(res.body, mockResponse);
}

TEST_F(RouteControllerUnitTests, AddFoodTest) {
  crow::request req;
  crow::response res;

  // Set valid API key for POST operations
  req.add_header("Authorization", "Bearer " + getValidTokenForPost());

  // Add all required fields
  req.body =
      "{\"FoodType\": \"Vegetables\", "
      "\"Provider\": \"Local Farm\", "
      "\"location\": \"NYC\", "
      "\"quantity\": \"100\", "
      "\"expirationDate\": \"2024-12-31\"}";

  ON_CALL(*mockFood, addFood(::testing::_))
      .WillByDefault(::testing::Return("Success"));

  routeController->addFood(req, res);

  EXPECT_EQ(res.code, 201);
}

TEST_F(RouteControllerUnitTests, GetAllFoodTestUnauthorized) {
  std::string mockResponse = R"([{"name": "FoodBank", "location": "NYC"}])";
  ON_CALL(*mockFood, getAllFood(0))
      .WillByDefault(::testing::Return(mockResponse));

  crow::request req{};
  crow::response res{};

  routeController->getAllFood(req, res);

  EXPECT_EQ(res.code, 401);
  EXPECT_EQ(res.body, "Authentication required. Please provide a valid token.");
}

TEST_F(RouteControllerUnitTests, AddFoodTestUnauthorized) {
  std::string input =
      R"({
    "Name" : "temp",
    "City" : "New York",
    "Address": "temp",
    "Description" : "NULL",
    "ContactInfo" : "66664566565",
    "HoursOfOperation": "2024-01-11",
    "TargetUser" :"HML",
    "Quantity" : "100",
    "ExpirationDate": "10"
  })";
  crow::request req;
  req.body = input;
  req.add_header("Authorization", "Bearer invalid.token.here");
  crow::response res{};

  ON_CALL(*mockFood, addFood(input)).WillByDefault(::testing::Return("12345"));

  routeController->addFood(req, res);

  EXPECT_EQ(res.code, 401);
  EXPECT_EQ(res.body, "Invalid or expired token.");
}
TEST_F(RouteControllerUnitTests, DeleteFoodTestAuthorized) {
  std::string body = R"({"id": "507f191e810c19729de860ea"})";
  crow::request req;
  req.add_header("Authorization", "Bearer " + getValidTokenForPost());
  req.body = body;
  crow::response res{};

  std::string id = "507f191e810c19729de860ea";

  ON_CALL(*mockFood, deleteFood(id))
      .WillByDefault(::testing::Return("Food resource deleted successfully."));

  routeController->deleteFood(req, res);

  EXPECT_EQ(res.code, 200);
  EXPECT_EQ(res.body, "Food resource deleted successfully.");
}

TEST_F(RouteControllerUnitTests, DeleteFoodTestUnauthorized) {
  std::string body = R"({"id": "507f191e810c19729de860ea"})";
  crow::request req;
  req.add_header("Authorization", "Bearer invalid.token.here");
  req.body = body;
  crow::response res{};

  routeController->deleteFood(req, res);

  EXPECT_EQ(res.code, 401);
  EXPECT_EQ(res.body, "Invalid or expired token.");
}
TEST_F(RouteControllerUnitTests, UpdateFoodTestAuthorized) {
  std::string body = "";
  crow::request req;
  req.add_header("Authorization", "Bearer " + getValidTokenForPost());
  req.body = body;
  crow::response res{};

  ON_CALL(*mockFood, updateFood(body))
      .WillByDefault(::testing::Return("Food resource updated successfully."));

  routeController->updateFood(req, res);

  EXPECT_EQ(res.code, 200);
  EXPECT_EQ(res.body, "Food resource updated successfully.");
}
TEST_F(RouteControllerUnitTests, GetAllOutreachServicesTestAuthorized) {
  std::string mockResponse = R"([{"programName": "OutreachProgram"}])";
  ON_CALL(*mockOutreach, getAllOutreachServices(0))
      .WillByDefault(::testing::Return(mockResponse));

  crow::request req{};
  req.add_header("Authorization", "Bearer " + getValidTokenForGet());
  crow::response res{};
  routeController->getAllOutreachServices(req, res);

  EXPECT_EQ(res.code, 200);
  EXPECT_EQ(res.body, mockResponse);
}

TEST_F(RouteControllerUnitTests, AddOutreachServiceTestAuthorized) {
  std::string body =
      R"({
    "Name":"Emergency Shelter Access",
    "City":"New York",
    "Address":"200 Varick St, New York, NY 10014",
    "Description":"Provide information and assistance for accessing shelters.",
    "ContactInfo":"Sarah Johnson, sarah@email.com",
    "HoursOfOperation":"05/01/24 - 12/31/24",
    "TargetAudience":"HML"
})";
  crow::request req;
  req.add_header("Authorization", "Bearer " + getValidTokenForPost());
  req.body = body;
  crow::response res{};

  ON_CALL(*mockOutreach, addOutreachService(body))
      .WillByDefault(::testing::Return("Success"));

  routeController->addOutreachService(req, res);

  EXPECT_EQ(res.code, 201);
  EXPECT_EQ(res.body, "Success");
}

TEST_F(RouteControllerUnitTests, GetAllOutreachServicesTestUnauthorized) {
  std::string mockResponse = R"([{"programName": "OutreachProgram"}])";
  ON_CALL(*mockOutreach, getAllOutreachServices(0))
      .WillByDefault(::testing::Return(mockResponse));

  crow::request req{};
  crow::response res{};

  routeController->getAllOutreachServices(req, res);

  EXPECT_EQ(res.code, 401);
  EXPECT_EQ(res.body, "Authentication required. Please provide a valid token.");
}

TEST_F(RouteControllerUnitTests, AddOutreachServiceTestUnauthorized) {
  std::string body =
      R"({
    "Name":"Emergency Shelter Access",
    "City":"New York",
    "Address":"200 Varick St, New York, NY 10014",
    "Description":"Provide information and assistance for accessing shelters.",
    "ContactInfo":"Sarah Johnson, sarah@email.com",
    "HoursOfOperation":"05/01/24 - 12/31/24",
    "TargetAudience":"HML"
})";
  crow::request req;
  req.body = body;
  req.add_header("Authorization", "Bearer invalid.token.here");
  crow::response res{};

  ON_CALL(*mockOutreach, addOutreachService(body))
      .WillByDefault(::testing::Return("Success"));

  routeController->addOutreachService(req, res);

  EXPECT_EQ(res.code, 401);
  EXPECT_EQ(res.body, "Invalid or expired token.");
}
TEST_F(RouteControllerUnitTests, DeleteOutreachServiceTestAuthorized) {
  std::string body = R"({"id": "507f191e810c19729de860ea"})";
  crow::request req;
  req.add_header("Authorization", "Bearer " + getValidTokenForPost());
  req.body = body;
  crow::response res{};

  std::string id = "507f191e810c19729de860ea";

  ON_CALL(*mockOutreach, deleteOutreach(id))
      .WillByDefault(
          ::testing::Return("Outreach resource deleted successfully."));

  routeController->deleteOutreach(req, res);

  EXPECT_EQ(res.code, 200);
  EXPECT_EQ(res.body, "Outreach resource deleted successfully.");
}

TEST_F(RouteControllerUnitTests, DeleteOutreachServiceTestUnauthorized) {
  std::string body = R"({"id": "507f191e810c19729de860ea"})";
  crow::request req;
  req.add_header("Authorization", "Bearer invalid.token.here");
  req.body = body;
  crow::response res{};

  routeController->deleteOutreach(req, res);

  EXPECT_EQ(res.code, 401);
  EXPECT_EQ(res.body, "Invalid or expired token.");
}
TEST_F(RouteControllerUnitTests, UpdateOutreachTestAuthorized) {
  std::string body = "";
  crow::request req;
  req.add_header("Authorization", "Bearer " + getValidTokenForPost());
  req.body = body;
  crow::response res{};

  ON_CALL(*mockOutreach, updateOutreach(body))
      .WillByDefault(
          ::testing::Return("Outreach resource update successfully."));

  routeController->updateOutreach(req, res);

  EXPECT_EQ(res.code, 200);
  EXPECT_EQ(res.body, "Outreach resource update successfully.");
}
TEST_F(RouteControllerUnitTests, GetAllHealthcareServicesTestAuthorized) {
  std::string mockResponse = R"([{"provider": "HealthcareProvider"}])";
  ON_CALL(*mockHealthcare, getAllHealthcareServices(0))
      .WillByDefault(::testing::Return(mockResponse));

  crow::request req{};
  req.add_header("Authorization", "Bearer " + getValidTokenForGet());
  crow::response res{};
  routeController->getAllHealthcareServices(req, res);

  EXPECT_EQ(res.code, 200);
  EXPECT_EQ(res.body, mockResponse);
}

TEST_F(RouteControllerUnitTests, AddHealthcareServiceTestAuthorized) {
  std::string body =
      R"({
  "Name": "My New Hospital",
  "City" : "New York",
  "Address": "456 New St",
  "Description": "General Checkup",
  "HoursOfOperation": "9 AM - 5 PM",
  "eligibilityCriteria": "Adults",
  "contactInfo": "123-456-7890"
})";
  crow::request req;
  req.add_header("Authorization", "Bearer " + getValidTokenForPost());
  req.body = body;
  crow::response res{};

  ON_CALL(*mockHealthcare, addHealthcareService(body))
      .WillByDefault(::testing::Return("Success"));

  routeController->addHealthcareService(req, res);

  EXPECT_EQ(res.code, 201);
}

TEST_F(RouteControllerUnitTests, GetAllHealthcareServicesTestUnauthorized) {
  std::string mockResponse = R"([{"provider": "HealthcareProvider"}])";
  ON_CALL(*mockHealthcare, getAllHealthcareServices(0))
      .WillByDefault(::testing::Return(mockResponse));

  crow::request req{};
  crow::response res{};

  routeController->getAllHealthcareServices(req, res);

  EXPECT_EQ(res.code, 401);
  EXPECT_EQ(res.body, "Authentication required. Please provide a valid token.");
}

TEST_F(RouteControllerUnitTests, AddHealthcareServiceTestUnauthorized) {
  std::string body = R"({
  "Name": "My New Hospital",
  "City" : "New York",
  "Address": "456 New St",
  "Description": "General Checkup",
  "HoursOfOperation": "9 AM - 5 PM",
  "eligibilityCriteria": "Adults",
  "contactInfo": "123-456-7890"
})";
  crow::request req;
  req.body = body;
  req.add_header("Authorization", "Bearer invalid.token.here");
  crow::response res{};

  ON_CALL(*mockHealthcare, addHealthcareService(body))
      .WillByDefault(::testing::Return("Success"));

  routeController->addHealthcareService(req, res);

  EXPECT_EQ(res.code, 401);
  EXPECT_EQ(res.body, "Invalid or expired token.");
}

TEST_F(RouteControllerUnitTests, UpdateHealthcareServiceTestAuthorized) {
  std::string body =
      R"({
     "id":"507f191e810c19729de860ea",
      "Name": "My New Hospital",
      "City" : "New York",
      "Address": "456 New St",
      "Description": "General Checkup",
      "HoursOfOperation": "9 AM - 5 PM",
      "eligibilityCriteria": "Adults",
      "contactInfo": "123-456-7890"
    })";
  crow::request req;
  req.add_header("Authorization", "Bearer " + getValidTokenForPost());
  req.body = body;
  crow::response res{};

  std::map<std::string, std::string> expectedContent = {
      {"provider", "City Hospital"}, {"serviceType", "Emergency"},
      {"location", "456 Elm St"},    {"city", "New York"},
      {"operatingHours", "24/7"},    {"contactInfo", "987-654-3210"}};
  std::string id = "507f191e810c19729de860ea";

  ON_CALL(*mockHealthcare, updateHealthcare(body))
      .WillByDefault(
          ::testing::Return("Healthcare resource update successfully."));

  routeController->updateHealthcareService(req, res);

  EXPECT_EQ(res.code, 200);
  EXPECT_EQ(res.body, "Healthcare resource update successfully.");
}

TEST_F(RouteControllerUnitTests, UpdateHealthcareServiceTestUnauthorized) {
  std::string body =
      R"({"id": "507f191e810c19729de860ea", "provider": "City Hospital", "serviceType": "Emergency", "location": "456 Elm St", "operatingHours": "24/7", "contactInfo": "987-654-3210"})";
  crow::request req;
  req.add_header("Authorization", "Bearer invalid.token.here");
  req.body = body;
  crow::response res{};

  routeController->updateHealthcareService(req, res);

  EXPECT_EQ(res.code, 401);
  EXPECT_EQ(res.body, "Invalid or expired token.");
}

TEST_F(RouteControllerUnitTests, DeleteHealthcareServiceTestAuthorized) {
  std::string body = R"({"id": "507f191e810c19729de860ea"})";
  crow::request req;
  req.add_header("Authorization", "Bearer " + getValidTokenForPost());
  req.body = body;
  crow::response res{};

  std::string id = "507f191e810c19729de860ea";

  ON_CALL(*mockHealthcare, deleteHealthcare(id))
      .WillByDefault(
          ::testing::Return("Healthcare record deleted successfully."));

  routeController->deleteHealthcareService(req, res);

  EXPECT_EQ(res.code, 200);
  EXPECT_EQ(res.body, "Healthcare record deleted successfully.");
}

TEST_F(RouteControllerUnitTests, DeleteHealthcareServiceTestUnauthorized) {
  std::string body = R"({"id": "507f191e810c19729de860ea"})";
  crow::request req;
  req.add_header("Authorization", "Bearer invalid.token.here");
  req.body = body;
  crow::response res{};

  routeController->deleteHealthcareService(req, res);

  EXPECT_EQ(res.code, 401);
  EXPECT_EQ(res.body, "Invalid or expired token.");
}

TEST_F(RouteControllerUnitTests, AddSubscriberTestAuthorized) {
  std::string body =
      R"({
        "City": "New York",
        "Resource": "Healthcare",
        "Contact": "subscriber@example.com"
      })";
  crow::request req;
  req.add_header("Authorization", "Bearer " + getValidTokenForPost());
  req.body = body;
  crow::response res{};

  std::map<std::string, std::string> subscriberDetails = {
      {"City", "New York"},
      {"Resource", "Healthcare"},
      {"Contact", "subscriber@example.com"}};

  std::string mockId = "507f191e810c19729de860ea";

  ON_CALL(*mockSubscriptionManager, addSubscriber(subscriberDetails))
      .WillByDefault(::testing::Return(mockId));

  routeController->subscribeToResources(req, res);

  EXPECT_EQ(res.code, 201);
  EXPECT_EQ(res.body, "Subscription recorded successfully.");
}

TEST_F(RouteControllerUnitTests, AddSubscriberTestUnauthorized) {
  std::string body =
      R"({
        "City": "New York",
        "Resource": "Healthcare",
        "Contact": "subscriber@example.com"
      })";
  crow::request req;
  req.add_header("Authorization", "Bearer invalid.token.here");
  req.body = body;
  crow::response res{};

  routeController->subscribeToResources(req, res);

  EXPECT_EQ(res.code, 401);
  EXPECT_EQ(res.body, "Invalid or expired token.");
}
