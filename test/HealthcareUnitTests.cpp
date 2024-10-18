#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Healthcare.h"
#include "DatabaseManager.h"
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

class MockDatabaseManager : public DatabaseManager {
public:
    MockDatabaseManager() : DatabaseManager("mongodb://localhost:27017", true) {}

    MOCK_METHOD(void, findCollection, 
        (const std::string& collectionName, 
        (const std::vector<std::pair<std::string, std::string>>& keyValues), 
        (std::vector<bsoncxx::document::value>& result)), (override));

    MOCK_METHOD(void, insertResource, 
        (const std::string& collectionName, 
        (const std::vector<std::pair<std::string, std::string>>& keyValues)), (override));
};


class HealthcareServiceUnitTests : public ::testing::Test {
protected:
    MockDatabaseManager* mockDbManager;
    HealthcareService* healthcareService;

    void SetUp() override {
        mockDbManager = new MockDatabaseManager();
        healthcareService = new HealthcareService(*mockDbManager, "HealthcareTest");
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

    EXPECT_CALL(*mockDbManager, findCollection(::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::DoAll(
            ::testing::SetArgReferee<2>(mockResult), 
            ::testing::Return()  
        ));

    std::string services = healthcareService->getAllHealthcareServices();

    EXPECT_NE(services, "[]");
    EXPECT_TRUE(services.find("City Hospital") != std::string::npos);
    EXPECT_TRUE(services.find("General Checkup") != std::string::npos);
    EXPECT_TRUE(services.find("123 Main St") != std::string::npos);
}

TEST_F(HealthcareServiceUnitTests, AddNewHealthcareService) {
    std::vector<std::pair<std::string, std::string>> expectedContent = {
        {"provider", "City Hospital"},
        {"serviceType", "General Checkup"},
        {"location", "123 Main St"},
        {"operatingHours", "9 AM - 5 PM"},
        {"eligibilityCriteria", "Adults"},
        {"contactInfo", "123-456-7890"}
    };

    EXPECT_CALL(*mockDbManager, insertResource(::testing::_, ::testing::_))
        .WillOnce(::testing::Invoke(
            [&](const std::string& collectionName, 
                const std::vector<std::pair<std::string, std::string>>& content) {
                EXPECT_EQ(collectionName, "HealthcareTest");
                EXPECT_EQ(content, expectedContent);
            }
        ));

    std::string result = healthcareService->addHealthcareService(
        "City Hospital", "General Checkup", "123 Main St", "9 AM - 5 PM", "Adults", "123-456-7890");

    EXPECT_EQ(result, "Success");
}