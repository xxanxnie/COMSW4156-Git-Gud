// Copyright 2024 COMSW4156-Git-Gud

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include "Auth.h"
#include "MockDatabaseManager.h"

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

TEST_F(AuthUnitTests, RegisterUser) {
  std::string email = "test@example.com";
  std::string password = "TestPass123";
  
  // Mock the database calls
  std::vector<bsoncxx::document::value> emptyResult;
  ON_CALL(*mockDbManager, findCollection(::testing::_, ::testing::_, ::testing::_, ::testing::_))
      .WillByDefault(::testing::DoAll(::testing::SetArgReferee<3>(emptyResult),
                                     ::testing::Return()));

  ON_CALL(*mockDbManager, insertResource(::testing::_, ::testing::_))
      .WillByDefault(::testing::Return("user_id_123"));

  // After insertion, mock the user retrieval
  std::vector<bsoncxx::document::value> mockResult;
  mockResult.push_back(bsoncxx::builder::stream::document{}
                      << "_id" << bsoncxx::oid{}
                      << "email" << email
                      << "passwordHash" << "hashed_password"
                      << "role" << "USER"
                      << "createdAt" << "timestamp"
                      << bsoncxx::builder::stream::finalize);

  ON_CALL(*mockDbManager, findCollection(::testing::_, ::testing::_, ::testing::_, ::testing::_))
      .WillByDefault(::testing::DoAll(::testing::SetArgReferee<3>(mockResult),
                                     ::testing::Return()));

  std::string token = authService->registerUser(email, password);
  EXPECT_FALSE(token.empty());
}

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

TEST_F(AuthUnitTests, RegisterUserAlreadyExists) {
  std::string email = "existing@example.com";
  std::string password = "TestPass123";

  // Mock existing user
  std::vector<bsoncxx::document::value> mockResult;
  mockResult.push_back(bsoncxx::builder::stream::document{}
                      << "email" << email
                      << "passwordHash" << "existing_hash"
                      << bsoncxx::builder::stream::finalize);

  ON_CALL(*mockDbManager, findCollection(::testing::_, ::testing::_, ::testing::_, ::testing::_))
      .WillByDefault(::testing::DoAll(::testing::SetArgReferee<3>(mockResult),
                                     ::testing::Return()));

  EXPECT_THROW(authService->registerUser(email, password), UserAlreadyExistsException);
}

TEST_F(AuthUnitTests, LoginUser) {
  std::string email = "test@example.com";
  std::string password = "TestPass123";
  std::string hashedPassword = authService->hashPassword(password);

  // Mock user retrieval
  std::vector<bsoncxx::document::value> mockResult;
  mockResult.push_back(bsoncxx::builder::stream::document{}
                      << "_id" << bsoncxx::oid{}
                      << "email" << email
                      << "passwordHash" << hashedPassword
                      << "role" << "USER"
                      << "createdAt" << "timestamp"
                      << bsoncxx::builder::stream::finalize);

  ON_CALL(*mockDbManager, findCollection(::testing::_, ::testing::_, ::testing::_, ::testing::_))
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
                      << "_id" << bsoncxx::oid{}
                      << "email" << email
                      << "passwordHash" << hashedPassword
                      << "role" << "USER"
                      << "createdAt" << "timestamp"
                      << bsoncxx::builder::stream::finalize);

  ON_CALL(*mockDbManager, findCollection(::testing::_, ::testing::_, ::testing::_, ::testing::_))
      .WillByDefault(::testing::DoAll(::testing::SetArgReferee<3>(mockResult),
                                     ::testing::Return()));

  EXPECT_THROW(authService->loginUser(email, password), InvalidCredentialsException);
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
