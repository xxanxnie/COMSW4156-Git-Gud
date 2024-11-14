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

  MOCK_METHOD(std::string, addShelter,
              (std::string ORG, std::string User, std::string location,
               int capacity, int curUse),
              (override));
  MOCK_METHOD(std::string, deleteShelter, (std::string id), (override));
  MOCK_METHOD(std::string, searchShelterAll, (), (override));
  MOCK_METHOD(std::string, updateShelter,
              (std::string id, std::string ORG, std::string User,
               std::string location, int capacity, int curUse),
              (override));
};

class MockCounseling : public Counseling {
 public:
  explicit MockCounseling(DatabaseManager* dbManager)
      : Counseling(*dbManager) {}

  MOCK_METHOD(std::string, addCounselor,
              (const std::string& counselorName, const std::string& specialty),
              (override));
  MOCK_METHOD(std::string, deleteCounselor, (const std::string& counselorId),
              (override));
  MOCK_METHOD(std::string, searchCounselorsAll, (), (override));
  MOCK_METHOD(std::string, updateCounselor,
              (const std::string& counselorId, const std::string& field,
               const std::string& value),
              (override));
};

class MockFood : public Food {
 public:
  explicit MockFood(DatabaseManager* db) : Food(*db) {}

  MOCK_METHOD(
      std::string, addFood,
      ((const std::vector<std::pair<std::string, std::string>>& resource)),
      (override));
  MOCK_METHOD(std::string, getAllFood, (), (override));
};

class MockOutreachService : public Outreach {
 public:
  MockOutreachService(DatabaseManager* dbManager,
                      const std::string& collection_name)
      : Outreach(*dbManager, collection_name) {}

  MOCK_METHOD(std::string, addOutreachService,
              (const std::string& targetAudience,
               const std::string& programName, const std::string& description,
               const std::string& programDate, const std::string& location,
               const std::string& contactInfo),
              (override));
  MOCK_METHOD(std::string, getAllOutreachServices, (), (override));
};

class MockHealthcareService : public Healthcare {
 public:
  MockHealthcareService(DatabaseManager* dbManager,
                        const std::string& collection_name)
      : Healthcare(*dbManager, collection_name) {}

  MOCK_METHOD(std::string, addHealthcareService,
            ((const std::map<std::string, std::string>& updates)),
            (override));
  MOCK_METHOD(std::string, getAllHealthcareServices, (), (override));

  MOCK_METHOD(std::string, updateHealthcare,
              (const std::string&, (const std::map<std::string, std::string>&)),
              (override));

  MOCK_METHOD(std::string, deleteHealthcare,
              ((std::string)),
              (override));
};

class RouteControllerUnitTests : public ::testing::Test {
 protected:
  MockDatabaseManager* mockDbManager;
  MockShelter* mockShelter;
  MockCounseling* mockCounseling;
  MockFood* mockFood;
  MockOutreachService* mockOutreach;
  MockHealthcareService* mockHealthcare;
  RouteController* routeController;

  void SetUp() override {
    mockDbManager = new MockDatabaseManager();
    mockShelter = new MockShelter(mockDbManager);
    mockCounseling = new MockCounseling(mockDbManager);
    mockFood = new MockFood(mockDbManager);
    mockOutreach =
        new MockOutreachService(mockDbManager, "OutreachServiceTest");
    mockHealthcare =
        new MockHealthcareService(mockDbManager, "HealthcareServiceTest");
    routeController =
        new RouteController(*mockDbManager, *mockShelter, *mockCounseling,
                            *mockHealthcare, *mockOutreach, *mockFood);
  }

  void TearDown() override {
    delete routeController;
    delete mockHealthcare;
    delete mockOutreach;
    delete mockFood;
    delete mockCounseling;
    delete mockShelter;
    delete mockDbManager;
  }
};

TEST_F(RouteControllerUnitTests, GetShelterTestAuthorized) {
  std::string mockResponse =
      R"([{"ORG": "NGO", "User": "HML", "location": "NYC"}])";
  ON_CALL(*mockShelter, searchShelterAll())
      .WillByDefault(::testing::Return(mockResponse));

  crow::request req{};
  req.add_header("API-Key", "hml345HML");
  crow::response res{};

  routeController->getShelter(req, res);

  EXPECT_EQ(res.code, 200);
  EXPECT_EQ(res.body, mockResponse);
}

TEST_F(RouteControllerUnitTests, AddShelterTestAuthorized) {
  std::string body =
      R"({"ORG": "NGO", "User": "HML", "location": "NYC", "capacity": "100", "curUse": "20"})";
  crow::request req;
  req.body = body;
  req.add_header("API-Key", "abc123NGO");
  crow::response res{};

  ON_CALL(*mockShelter, addShelter("NGO", "HML", "NYC", 100, 20))
      .WillByDefault(::testing::Return("Success"));

  routeController->addShelter(req, res);

  EXPECT_EQ(res.code, 201);
  EXPECT_EQ(res.body, "Shelter resource added successfully.");
}

TEST_F(RouteControllerUnitTests, GetShelterTestUnauthorized) {
  std::string mockResponse =
      R"([{"ORG": "NGO", "User": "HML", "location": "NYC"}])";
  ON_CALL(*mockShelter, searchShelterAll())
      .WillByDefault(::testing::Return(mockResponse));

  crow::request req{};
  req.add_header("API-Key", "invalid");
  crow::response res{};

  routeController->getShelter(req, res);

  EXPECT_EQ(res.code, 403);
  EXPECT_EQ(res.body, "Unauthorized.");
}

TEST_F(RouteControllerUnitTests, AddShelterTestUnauthorized) {
  std::string body =
      R"({"ORG": "NGO", "User": "HML", "location": "NYC", "capacity": "100", "curUse": "20"})";
  crow::request req;
  req.body = body;
  req.add_header("API-Key", "invalid");
  crow::response res{};

  ON_CALL(*mockShelter, addShelter("NGO", "HML", "NYC", 100, 20))
      .WillByDefault(::testing::Return("Success"));

  routeController->addShelter(req, res);

  EXPECT_EQ(res.code, 403);
  EXPECT_EQ(res.body, "Unauthorized.");
}

TEST_F(RouteControllerUnitTests, GetCounselingTestAuthorized) {
  std::string mockResponse = R"([{"name": "John Doe", "specialty": "CBT"}])";
  ON_CALL(*mockCounseling, searchCounselorsAll())
      .WillByDefault(::testing::Return(mockResponse));

  crow::request req{};
  req.add_header("API-Key", "rfg678RFG");
  crow::response res{};
  routeController->getCounseling(req, res);

  EXPECT_EQ(res.code, 200);
  EXPECT_EQ(res.body, mockResponse);
}

TEST_F(RouteControllerUnitTests, AddCounselingTestAuthorized) {
  std::string body =
      R"({"counselorName": "Jane Smith", "specialty": "Family Therapy"})";
  crow::request req;
  req.body = body;
  req.add_header("API-Key", "def456VOL");
  crow::response res{};

  ON_CALL(*mockCounseling, addCounselor("Jane Smith", "Family Therapy"))
      .WillByDefault(::testing::Return("Success"));

  routeController->addCounseling(req, res);

  EXPECT_EQ(res.code, 201);
  EXPECT_EQ(res.body, "Counseling resource added successfully.");
}

TEST_F(RouteControllerUnitTests, GetCounselingTestUnauthorized) {
  std::string mockResponse = R"([{"name": "John Doe", "specialty": "CBT"}])";
  ON_CALL(*mockCounseling, searchCounselorsAll())
      .WillByDefault(::testing::Return(mockResponse));

  crow::request req{};
  req.add_header("API-Key", "invalid");
  crow::response res{};

  routeController->getCounseling(req, res);

  EXPECT_EQ(res.code, 403);
  EXPECT_EQ(res.body, "Unauthorized.");
}

TEST_F(RouteControllerUnitTests, AddCounselingTestUnauthorized) {
  std::string body =
      R"({"counselorName": "Jane Smith", "specialty": "Family Therapy"})";
  crow::request req;
  req.body = body;
  req.add_header("API-Key", "invalid");
  crow::response res{};

  ON_CALL(*mockCounseling, addCounselor("Jane Smith", "Family Therapy"))
      .WillByDefault(::testing::Return("Success"));

  routeController->addCounseling(req, res);

  EXPECT_EQ(res.code, 403);
  EXPECT_EQ(res.body, "Unauthorized.");
}

TEST_F(RouteControllerUnitTests, GetAllFoodTestAuthorized) {
  crow::request req;
  crow::response res;
  
  // Set valid API key for GET operations
  req.add_header("API-Key", "hml345HML");

  std::string mockResponse = "[{\"name\": \"FoodBank\", \"location\": \"NYC\"}]";
  ON_CALL(*mockFood, getAllFood())
      .WillByDefault(::testing::Return(mockResponse));

  routeController->getAllFood(req, res);

  EXPECT_EQ(res.code, 200);
  EXPECT_EQ(res.body, mockResponse);
}

TEST_F(RouteControllerUnitTests, AddFoodTest) {
  crow::request req;
  crow::response res;
  
  // Set valid API key for POST operations
  req.add_header("API-Key", "abc123NGO");
  
  // Add all required fields
  req.body = "{\"FoodType\": \"Vegetables\", "
             "\"Provider\": \"Local Farm\", "
             "\"location\": \"NYC\", "
             "\"quantity\": \"100\", "
             "\"expirationDate\": \"2024-12-31\"}";

  ON_CALL(*mockFood, addFood(::testing::_))
      .WillByDefault(::testing::Return("Success"));

  routeController->addFood(req, res);

  EXPECT_EQ(res.code, 201);
  EXPECT_EQ(res.body, "Food resource added successfully.");
}

TEST_F(RouteControllerUnitTests, GetAllFoodTestUnauthorized) {
  std::string mockResponse = R"([{"name": "FoodBank", "location": "NYC"}])";
  ON_CALL(*mockFood, getAllFood())
      .WillByDefault(::testing::Return(mockResponse));

  crow::request req{};
  req.add_header("API-Key", "invalid");
  crow::response res{};

  routeController->getAllFood(req, res);

  EXPECT_EQ(res.code, 403);
  EXPECT_EQ(res.body, "Unauthorized.");
}

TEST_F(RouteControllerUnitTests, AddFoodTestUnauthorized) {
  std::string body = R"({"name": "FoodBank", "location": "NYC"})";

  std::vector<std::pair<std::string, std::string>> expectedContent = {
      {"name", "FoodBank"}, {"location", "NYC"}};

  ON_CALL(*mockFood, addFood(expectedContent))
      .WillByDefault(::testing::Return("Success"));

  crow::request req;
  req.body = body;
  req.add_header("API-Key", "invalid");
  crow::response res{};

  routeController->addFood(req, res);

  EXPECT_EQ(res.code, 403);
  EXPECT_EQ(res.body, "Unauthorized.");
}

TEST_F(RouteControllerUnitTests, GetAllOutreachServicesTestAuthorized) {
  std::string mockResponse = R"([{"programName": "OutreachProgram"}])";
  ON_CALL(*mockOutreach, getAllOutreachServices())
      .WillByDefault(::testing::Return(mockResponse));

  crow::request req{};
  req.add_header("API-Key", "hml345HML");
  crow::response res{};
  routeController->getAllOutreachServices(req, res);

  EXPECT_EQ(res.code, 200);
  EXPECT_EQ(res.body, mockResponse);
}

TEST_F(RouteControllerUnitTests, AddOutreachServiceTestAuthorized) {
  std::string body =
      R"({"targetAudience": "Youth", "programName": "OutreachProgram", "description": "Help", "programDate": "2023-01-01", "location": "NYC", "contactInfo": "123-456"})";
  crow::request req;
  req.add_header("API-Key", "def456VOL");
  req.body = body;
  crow::response res{};

  ON_CALL(*mockOutreach, addOutreachService("Youth", "OutreachProgram", "Help",
                                            "2023-01-01", "NYC", "123-456"))
      .WillByDefault(::testing::Return("Success"));

  routeController->addOutreachService(req, res);

  EXPECT_EQ(res.code, 201);
  EXPECT_EQ(res.body, "OutreachService resource added successfully.");
}

TEST_F(RouteControllerUnitTests, GetAllOutreachServicesTestUnauthorized) {
  std::string mockResponse = R"([{"programName": "OutreachProgram"}])";
  ON_CALL(*mockOutreach, getAllOutreachServices())
      .WillByDefault(::testing::Return(mockResponse));

  crow::request req{};
  req.add_header("API-Key", "invalid");
  crow::response res{};

  routeController->getAllOutreachServices(req, res);

  EXPECT_EQ(res.code, 403);
  EXPECT_EQ(res.body, "Unauthorized.");
}

TEST_F(RouteControllerUnitTests, AddOutreachServiceTestUnauthorized) {
  std::string body =
      R"({"targetAudience": "Youth", "programName": "OutreachProgram", "description": "Help", "programDate": "2023-01-01", "location": "NYC", "contactInfo": "123-456"})";
  crow::request req;
  req.body = body;
  req.add_header("API-Key", "invalid");
  crow::response res{};

  ON_CALL(*mockOutreach, addOutreachService("Youth", "OutreachProgram", "Help",
                                            "2023-01-01", "NYC", "123-456"))
      .WillByDefault(::testing::Return("Success"));

  routeController->addOutreachService(req, res);

  EXPECT_EQ(res.code, 403);
  EXPECT_EQ(res.body, "Unauthorized.");
}

TEST_F(RouteControllerUnitTests, GetAllHealthcareServicesTestAuthorized) {
  std::string mockResponse = R"([{"provider": "HealthcareProvider"}])";
  ON_CALL(*mockHealthcare, getAllHealthcareServices())
      .WillByDefault(::testing::Return(mockResponse));

  crow::request req{};
  req.add_header("API-Key", "rfg678RFG");
  crow::response res{};
  routeController->getAllHealthcareServices(req, res);

  EXPECT_EQ(res.code, 200);
  EXPECT_EQ(res.body, mockResponse);
}

TEST_F(RouteControllerUnitTests, AddHealthcareServiceTestAuthorized) {
  std::string body =
      R"({"provider": "HealthcareProvider", "serviceType": "General Care", "location": "NYC", "operatingHours": "9-5", "eligibilityCriteria": "None", "contactInfo": "123-456"})";
  crow::request req;
  req.add_header("API-Key", "ghi789CLN");
  req.body = body;
  crow::response res{};

  std::map<std::string, std::string> expectedContent = {
      {"provider", "HealthcareProvider"},
      {"serviceType", "General Care"},
      {"location", "NYC"},
      {"operatingHours", "9-5"},
      {"eligibilityCriteria", "None"},
      {"contactInfo", "123-456"}
  };

  ON_CALL(*mockHealthcare, addHealthcareService(expectedContent))
      .WillByDefault(::testing::Return("Success"));

  routeController->addHealthcareService(req, res);

  EXPECT_EQ(res.code, 201);
  EXPECT_EQ(res.body, "HealthcareService resource added successfully.");
}

TEST_F(RouteControllerUnitTests, GetAllHealthcareServicesTestUnauthorized) {
  std::string mockResponse = R"([{"provider": "HealthcareProvider"}])";
  ON_CALL(*mockHealthcare, getAllHealthcareServices())
      .WillByDefault(::testing::Return(mockResponse));

  crow::request req{};
  req.add_header("API-Key", "invalid");
  crow::response res{};

  routeController->getAllHealthcareServices(req, res);

  EXPECT_EQ(res.code, 403);
  EXPECT_EQ(res.body, "Unauthorized.");
}

TEST_F(RouteControllerUnitTests, AddHealthcareServiceTestUnauthorized) {
  std::string body =
      R"({"provider": "HealthcareProvider", "serviceType": "General Care", "location": "NYC", "operatingHours": "9-5", "eligibilityCriteria": "None", "contactInfo": "123-456"})";
  crow::request req;
  req.body = body;
  req.add_header("API-Key", "invalid");
  crow::response res{};

  std::map<std::string, std::string> expectedContent = {
      {"provider", "HealthcareProvider"},
      {"serviceType", "General Care"},
      {"location", "NYC"},
      {"operatingHours", "9-5"},
      {"eligibilityCriteria", "None"},
      {"contactInfo", "123-456"}
  };

  ON_CALL(*mockHealthcare, addHealthcareService(expectedContent))
      .WillByDefault(::testing::Return("Success"));

  routeController->addHealthcareService(req, res);

  EXPECT_EQ(res.code, 403);
  EXPECT_EQ(res.body, "Unauthorized.");
}

TEST_F(RouteControllerUnitTests, UpdateHealthcareServiceTestAuthorized) {
  std::string body = R"({"id": "507f191e810c19729de860ea", "provider": "City Hospital", "serviceType": "Emergency", "location": "456 Elm St", "operatingHours": "24/7", "contactInfo": "987-654-3210"})";
  crow::request req;
  req.add_header("API-Key", "ghi789CLN");
  req.body = body;
  crow::response res{};

  std::map<std::string, std::string> expectedContent = {
      {"provider", "City Hospital"},
      {"serviceType", "Emergency"},
      {"location", "456 Elm St"},
      {"operatingHours", "24/7"},
      {"contactInfo", "987-654-3210"}
  };
  std::string id = "507f191e810c19729de860ea";

  ON_CALL(*mockHealthcare, updateHealthcare(id, expectedContent))
      .WillByDefault(::testing::Return("Healthcare record updated successfully."));

  routeController->updateHealthcareService(req, res);

  EXPECT_EQ(res.code, 201);
  EXPECT_EQ(res.body, "Healthcare record updated successfully.");
}

TEST_F(RouteControllerUnitTests, UpdateHealthcareServiceTestUnauthorized) {
  std::string body = R"({"id": "507f191e810c19729de860ea", "provider": "City Hospital", "serviceType": "Emergency", "location": "456 Elm St", "operatingHours": "24/7", "contactInfo": "987-654-3210"})";
  crow::request req;
  req.add_header("API-Key", "invalid");
  req.body = body;
  crow::response res{};

  routeController->updateHealthcareService(req, res);

  EXPECT_EQ(res.code, 403);
  EXPECT_EQ(res.body, "Unauthorized.");
}

TEST_F(RouteControllerUnitTests, DeleteHealthcareServiceTestAuthorized) {
  std::string body = R"({"id": "507f191e810c19729de860ea"})";
  crow::request req;
  req.add_header("API-Key", "ghi789CLN");
  req.body = body;
  crow::response res{};

  std::string id = "507f191e810c19729de860ea";

  ON_CALL(*mockHealthcare, deleteHealthcare(id))
      .WillByDefault(::testing::Return("Healthcare record deleted successfully."));

  routeController->deleteHealthcareService(req, res);

  EXPECT_EQ(res.code, 201);
  EXPECT_EQ(res.body, "Healthcare record deleted successfully.");
}

TEST_F(RouteControllerUnitTests, DeleteHealthcareServiceTestUnauthorized) {
  std::string body = R"({"id": "507f191e810c19729de860ea"})";
  crow::request req;
  req.add_header("API-Key", "invalid");
  req.body = body;
  crow::response res{};

  routeController->deleteHealthcareService(req, res);

  EXPECT_EQ(res.code, 403);
  EXPECT_EQ(res.body, "Unauthorized.");
}

