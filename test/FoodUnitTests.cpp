#include <gtest/gtest.h>
#include "Food.h"

class FoodUnitTests : public ::testing::Test {
protected:
    static Food* food;

    static void SetUpTestSuite() {
        food = new Food("Bread", "10 loaves");
    }

    static void TearDownTestSuite() {
        delete food;
    }
};

// Example test case for Food type
TEST_F(FoodUnitTests, GetType) {
    EXPECT_EQ(food->getType(), "Bread");
}

// Example test case for Food quantity
TEST_F(FoodUnitTests, GetQuantity) {
    EXPECT_EQ(food->getQuantity(), "10 loaves");
}

Food* FoodUnitTests::food = nullptr;
