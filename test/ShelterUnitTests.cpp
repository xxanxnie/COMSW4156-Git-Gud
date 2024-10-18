// Copyright 2024 Wilson, Liang
#include <gtest/gtest.h>

#include "Shelter.h"

class ShelterUnitTests : public ::testing::Test {
 protected:
  static Shelter* shelter;
  static DatabaseManager* dbManager;
  static void SetUpTestSuite() {
    dbManager = new DatabaseManager("mongodb://localhost:27017");
    shelter = new Shelter(*dbManager, "ShelterTest");
  }
  static void DeleteDB() {
    dbManager->deleteCollection(shelter->collection_name);
  }
  static void TearDownTestSuite() {
    delete shelter;
    delete dbManager;
  }
};

// Example test case for Shelter
TEST_F(ShelterUnitTests, InsertShelter) {
  std::string ret = shelter->addShelter("tmp", "tmp", "tmp", 1, 0);
  EXPECT_EQ(ret, "Success");
}

// Example test case for Shelter location
TEST_F(ShelterUnitTests, searchShelterAll) {
  EXPECT_EQ(shelter->searchShelterAll(), "tmp tmp tmp 1 0 \n");
  DeleteDB();
}

Shelter* ShelterUnitTests::shelter = nullptr;
DatabaseManager* ShelterUnitTests::dbManager = nullptr;
