#include <gtest/gtest.h>
#include "Outreach.h"

class OutreachUnitTests : public ::testing::Test {
protected:
    static Outreach* outreach;

    static void SetUpTestSuite() {
        outreach = new Outreach("Community Support", "Low-income families");
    }

    static void TearDownTestSuite() {
        delete outreach;
    }
};

// Example test case for Outreach program name
TEST_F(OutreachUnitTests, GetProgramName) {
    EXPECT_EQ(outreach->getProgramName(), "Community Support");
}

// Example test case for Outreach target group
TEST_F(OutreachUnitTests, GetTargetGroup) {
    EXPECT_EQ(outreach->getTargetGroup(), "Low-income families");
}
