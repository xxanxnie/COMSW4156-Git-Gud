#include <gtest/gtest.h>

#include "Outreach.h"
#include "DatabaseManager.h"

class OutreachUnitTests : public ::testing::Test {
protected:
    static DatabaseManager* dbManager;

    static void SetUpTestSuite() {
        dbManager = new DatabaseManager("mongodb://localhost:27017");
        dbManager->createCollection("OutreachPrograms");
    }

    static void TearDownTestSuite() {
        dbManager->deleteResource("OutreachPrograms", "1"); // Clean up the test data
        delete dbManager;
    }
};

DatabaseManager* OutreachUnitTests::dbManager = nullptr;

// Test case to check the Outreach object creation and getters
TEST_F(OutreachUnitTests, OutreachGetters) {
    Outreach outreach(
        1, 
        "Homeless", 
        "Emergency Shelter Access", 
        "Provide information and assistance for accessing shelters.", 
        "2024-05-01", 
        "2024-12-31", 
        "Bowery Mission, 227 Bowery, NY", 
        "Sarah Johnson, sarah@email.com", 
        *dbManager
    );

    EXPECT_EQ(outreach.getID(), 1);
    EXPECT_EQ(outreach.getTargetAudience(), "Homeless");
    EXPECT_EQ(outreach.getProgramName(), "Emergency Shelter Access");
    EXPECT_EQ(outreach.getDescription(), "Provide information and assistance for accessing shelters.");
    EXPECT_EQ(outreach.getStartDate(), "2024-05-01");
    EXPECT_EQ(outreach.getEndDate(), "2024-12-31");
    EXPECT_EQ(outreach.getLocation(), "Bowery Mission, 227 Bowery, NY");
    EXPECT_EQ(outreach.getContactInfo(), "Sarah Johnson, sarah@email.com");
}

// Test case to check insertion into the database
TEST_F(OutreachUnitTests, InsertOutreach) {
    Outreach outreach(
        1, 
        "Homeless", 
        "Emergency Shelter Access", 
        "Provide information and assistance for accessing shelters.", 
        "2024-05-01", 
        "2024-12-31", 
        "Bowery Mission, 227 Bowery, NY", 
        "Sarah Johnson, sarah@email.com", 
        *dbManager
    );

    // Insert the outreach into the database using addOutreach
    outreach.addOutreach(
        "Homeless", 
        "Emergency Shelter Access", 
        "Provide information and assistance for accessing shelters.", 
        "2024-05-01", 
        "2024-12-31", 
        "Bowery Mission, 227 Bowery, NY", 
        "Sarah Johnson, sarah@email.com"
    );

    auto resources = dbManager->getResources("OutreachPrograms");

    bool found = false;
    for (const auto& doc : resources.view()["resources"].get_array().value) {
        if (doc["_id"].type() == bsoncxx::type::k_utf8) {
            auto id_view = doc["_id"].get_string(); 
            std::string id{id_view.value}; 
            if (id == "1") { 
                found = true;
                break;
            }
        }
    }
    EXPECT_TRUE(found); // Verify that the outreach program was inserted
}

// Test case to check database cleanup
TEST_F(OutreachUnitTests, Cleanup) {
    auto resources = dbManager->getResources("OutreachPrograms");
    
    // Check if the resources array exists and is empty
    EXPECT_TRUE(resources.view()["resources"].get_array().value.empty());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
