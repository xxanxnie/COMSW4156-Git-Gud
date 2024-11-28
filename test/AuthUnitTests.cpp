// Copyright 2024 COMSW4156-Git-Gud

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include "Auth.h"
#include "MockDatabaseManager.h"

std::string getValidTokenForGet() {
  return "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9."
         "eyJlbWFpbCI6ImFkYWFAZ21haWwuY29tIiwiZXhwIjoyNTk2NjgwMDI3LCJpYXQiOjE3Mz"
         "I2ODAwMjcsImlzcyI6ImF1dGgtc2VydmljZSIsInJvbGUiOiJITUwiLCJ1c2VySWQiOiI2"
         "NzQ2OTk1YjFiZmFiODQ2NDEwNjZjNjMifQ."
         "N0l6jhy5WfHEQCqq82OMPsoSPFobNMlyEHQ0M3Qo87A";
}

std::string getValidTokenForPost() {
  return "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9."
         "eyJlbWFpbCI6ImFkYUBnbWFpbC5jb20iLCJleHAiOjI1OTY2Nzk5OTAsImlhdCI6MTczMj"
         "Y3OTk5MCwiaXNzIjoiYXV0aC1zZXJ2aWNlIiwicm9sZSI6Ik5HTyIsInVzZXJJZCI6IjY3"
         "NDY5OTM2MWJmYWI4NDY0MTA2NmM2MiJ9.HrxegAGsSbQqX8h1m3F-o8fkuf4-"
         "j2q6qgA7pOYolwc";
}

class AuthUnitTests : public ::testing::Test {
 protected:
  MockDatabaseManager* mockDbManager;
  AuthService* authService;

  void SetUp() override {
    mockDbManager = new MockDatabaseManager();
    authService = new AuthService(*mockDbManager);
  }

  void TearDown() override {
    delete authService;
    delete mockDbManager;
  }
};

TEST_F(AuthUnitTests, RegisterUserInvalidEmail) {
  std::string email = "invalid-email";
  std::string password = "TestPass123";

  EXPECT_THROW(authService->registerUser(email, password), AuthException);
}

TEST_F(AuthUnitTests, RegisterUserInvalidPassword) {
  std::string email = "test@example.com";
  std::string password = "weak";

  EXPECT_THROW(authService->registerUser(email, password), AuthException);
}

TEST_F(AuthUnitTests, LoginUser) {
  std::string email = "test@example.com";
  std::string password = "TestPass123";
  std::string hashedPassword = authService->hashPassword(password);

  // Mock user retrieval
  std::vector<bsoncxx::document::value> mockResult;
  mockResult.push_back(bsoncxx::builder::stream::document{}
                       << "_id" << bsoncxx::oid{} << "email" << email
                       << "passwordHash" << hashedPassword << "role" << "USER"
                       << "createdAt" << "timestamp"
                       << bsoncxx::builder::stream::finalize);

  ON_CALL(*mockDbManager, findCollection(::testing::_, ::testing::_,
                                         ::testing::_, ::testing::_))
      .WillByDefault(::testing::DoAll(::testing::SetArgReferee<3>(mockResult),
                                      ::testing::Return()));

  std::string token = authService->loginUser(email, password);
  EXPECT_FALSE(token.empty());
}

TEST_F(AuthUnitTests, LoginUserInvalidCredentials) {
  std::string email = "test@example.com";
  std::string password = "WrongPass123";
  std::string hashedPassword = authService->hashPassword("DifferentPass123");

  // Mock user retrieval
  std::vector<bsoncxx::document::value> mockResult;
  mockResult.push_back(bsoncxx::builder::stream::document{}
                       << "_id" << bsoncxx::oid{} << "email" << email
                       << "passwordHash" << hashedPassword << "role" << "USER"
                       << "createdAt" << "timestamp"
                       << bsoncxx::builder::stream::finalize);

  ON_CALL(*mockDbManager, findCollection(::testing::_, ::testing::_,
                                         ::testing::_, ::testing::_))
      .WillByDefault(::testing::DoAll(::testing::SetArgReferee<3>(mockResult),
                                      ::testing::Return()));

  EXPECT_THROW(authService->loginUser(email, password),
               InvalidCredentialsException);
}

TEST_F(AuthUnitTests, VerifyJWT) {
  User testUser;
  testUser.id = "user_id_123";
  testUser.email = "test@example.com";
  testUser.role = "USER";

  std::string token = authService->generateJWT(testUser);
  EXPECT_TRUE(authService->verifyJWT(token));
}

TEST_F(AuthUnitTests, VerifyInvalidJWT) {
  std::string invalidToken = "invalid.token.here";
  EXPECT_FALSE(authService->verifyJWT(invalidToken));
}

TEST_F(AuthUnitTests, DecodeJWT) {
  User testUser;
  testUser.id = "user_id_123";
  testUser.email = "test@example.com";
  testUser.role = "USER";

  std::string token = authService->generateJWT(testUser);
  auto payload = authService->decodeJWT(token);

  EXPECT_TRUE(payload.has_value());
  EXPECT_EQ(payload->userId, testUser.id);
  EXPECT_EQ(payload->email, testUser.email);
  EXPECT_EQ(payload->role, testUser.role);
}

TEST_F(AuthUnitTests, HasRole) {
  User testUser;
  testUser.id = "user_id_123";
  testUser.email = "test@example.com";
  testUser.role = "NGO";

  std::string token = authService->generateJWT(testUser);
  EXPECT_TRUE(authService->hasRole(token, "NGO"));
  EXPECT_FALSE(authService->hasRole(token, "VOL"));
}

TEST_F(AuthUnitTests, ValidateEmail) {
  EXPECT_TRUE(authService->isValidEmail("test@example.com"));
  EXPECT_FALSE(authService->isValidEmail("invalid-email"));
  EXPECT_FALSE(authService->isValidEmail("test@.com"));
  EXPECT_FALSE(authService->isValidEmail("test@com"));
}

TEST_F(AuthUnitTests, ValidatePassword) {
  EXPECT_TRUE(authService->isValidPassword("TestPass123"));
  EXPECT_FALSE(authService->isValidPassword("weak"));
  EXPECT_FALSE(authService->isValidPassword("NoDigits"));
  EXPECT_FALSE(authService->isValidPassword("nocaps123"));
  EXPECT_FALSE(authService->isValidPassword("NOCAPS123"));
}

// Test getCurrentTimestamp and getExpirationTimestamp
TEST_F(AuthUnitTests, TimestampTests) {
  // Test getCurrentTimestamp
  int64_t timestamp1 = authService->getCurrentTimestamp();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  int64_t timestamp2 = authService->getCurrentTimestamp();

  EXPECT_GT(timestamp2, timestamp1);  // Second timestamp should be greater

  // Test getExpirationTimestamp
  int64_t currentTime = authService->getCurrentTimestamp();
  int64_t expirationTime = authService->getExpirationTimestamp();

  // Expiration should be JWT_EXPIRATION_HOURS hours ahead
  EXPECT_EQ(expirationTime - currentTime,
            24 * 36000000);  // Assuming JWT_EXPIRATION_HOURS is 24
}

// Test createUserDocument
TEST_F(AuthUnitTests, CreateUserDocumentTest) {
  std::string email = "test@example.com";
  std::string passwordHash = "hashedpassword123";
  std::string role = "USER";

  auto document = authService->createUserDocument(email, passwordHash, role);

  // Check all fields are present and correct
  EXPECT_EQ(document.size(), 4);  // Should have 4 fields

  // Check each field
  EXPECT_EQ(document[0].first, "email");
  EXPECT_EQ(document[0].second, email);

  EXPECT_EQ(document[1].first, "passwordHash");
  EXPECT_EQ(document[1].second, passwordHash);

  EXPECT_EQ(document[2].first, "role");
  EXPECT_EQ(document[2].second, role);

  EXPECT_EQ(document[3].first, "createdAt");
  // Verify createdAt is a valid timestamp (should be close to current time)
  int64_t createdAt = std::stoll(document[3].second);
  int64_t currentTime = authService->getCurrentTimestamp();
  EXPECT_NEAR(createdAt, currentTime, 2);  // Within 2 seconds
}

// Test authenticateToken
TEST_F(AuthUnitTests, AuthenticateTokenTest) {
  // Test valid token
  crow::request reqValid;
  reqValid.add_header("Authorization", "Bearer " + getValidTokenForGet());
  EXPECT_TRUE(authenticateToken(reqValid));

  // Test invalid token
  crow::request reqInvalid;
  reqInvalid.add_header("Authorization", "Bearer invalid.token.here");
  EXPECT_FALSE(authenticateToken(reqInvalid));

  // Test missing token
  crow::request reqMissing;
  EXPECT_FALSE(authenticateToken(reqMissing));

  // Test malformed header
  crow::request reqMalformed;
  reqMalformed.add_header("Authorization", "malformed_header");
  EXPECT_FALSE(authenticateToken(reqMalformed));
}

// Test authorizeRole
TEST_F(AuthUnitTests, AuthorizeRoleTest) {
  // Test HML role with GET request
  crow::request reqHML;
  reqHML.method = crow::HTTPMethod::GET;
  reqHML.add_header("Authorization", "Bearer " + getValidTokenForGet());
  EXPECT_TRUE(authorizeRole(reqHML, "HML"));
  EXPECT_FALSE(authorizeRole(reqHML, "NGO"));

  // Test NGO role with POST request
  crow::request reqNGO;
  reqNGO.method = crow::HTTPMethod::POST;
  reqNGO.add_header("Authorization", "Bearer " + getValidTokenForPost());
  EXPECT_TRUE(authorizeRole(reqNGO, "NGO"));
  EXPECT_FALSE(authorizeRole(reqNGO, "HML"));

  // Test invalid token
  crow::request reqInvalid;
  reqInvalid.add_header("Authorization", "Bearer invalid.token.here");
  EXPECT_FALSE(authorizeRole(reqInvalid, "HML"));

  // Test missing token
  crow::request reqMissing;
  EXPECT_FALSE(authorizeRole(reqMissing, "NGO"));
}

// Test token extraction helper
TEST_F(AuthUnitTests, ExtractTokenTest) {
  // Valid Bearer token
  std::string validHeader = "Bearer " + getValidTokenForGet();
  EXPECT_EQ(extractToken(validHeader), getValidTokenForPost());

  // Invalid format
  EXPECT_TRUE(extractToken("InvalidFormat").empty());

  // Empty header
  EXPECT_TRUE(extractToken("").empty());

  // Bearer with no token
  EXPECT_TRUE(extractToken("Bearer ").empty());
}
