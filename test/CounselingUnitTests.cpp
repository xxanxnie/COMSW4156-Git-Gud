#include <gtest/gtest.h>
#include "Counseling.h"

class CounselingUnitTests : public ::testing::Test {
protected:
    static Counseling* counseling;

    static void SetUpTestSuite() {
        counseling = new Counseling("Dr. Smith", "Mental Health");
    }

    static void TearDownTestSuite() {
        delete counseling;
    }
};

// Example test case for Counseling counselor name
TEST_F(CounselingUnitTests, GetCounselorName) {
    EXPECT_EQ(counseling->getCounselorName(), "Dr. Smith");
}

// Example test case for Counseling specialty
TEST_F(CounselingUnitTests, GetSpecialty) {
    EXPECT_EQ(counseling->getSpecialty(), "Mental Health");
}
