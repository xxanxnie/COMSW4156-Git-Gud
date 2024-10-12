#include <gtest/gtest.h>
#include "Healthcare.h"

class HealthcareUnitTests : public ::testing::Test {
protected:
    static Healthcare* healthcare;

    static void SetUpTestSuite() {
        healthcare = new Healthcare("City Hospital", "General Checkup");
    }

    static void TearDownTestSuite() {
        delete healthcare;
    }
};

// Example test case for Healthcare provider
TEST_F(HealthcareUnitTests, GetProvider) {
    EXPECT_EQ(healthcare->getProvider(), "City Hospital");
}

// Example test case for Healthcare service type
TEST_F(HealthcareUnitTests, GetServiceType) {
    EXPECT_EQ(healthcare->getServiceType(), "General Checkup");
}

Healthcare* HealthcareUnitTests::healthcare = nullptr;