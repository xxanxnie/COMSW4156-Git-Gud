#include "Auth.h"

#include <jwt-cpp/jwt.h>

#include <bcrypt/BCrypt.hpp>
#include <bsoncxx/json.hpp>
#include <chrono>
#include <regex>

// Constructor
AuthService::AuthService(DatabaseManager& dbManager) : dbManager(dbManager) {}

// Default constructor implementation
AuthService::AuthService() : dbManager(DatabaseManager::getInstance()) {}

// Registration
std::string AuthService::registerUser(const std::string& email,
                                      const std::string& password,
                                      const std::string& role) {
  // Validate input
  if (!isValidEmail(email)) {
    throw AuthException("Invalid email format");
  }
  if (!isValidPassword(password)) {
    throw AuthException("Password does not meet requirements");
  }

  // Check if user already exists
  if (findUserByEmail(email)) {
    throw UserAlreadyExistsException();
  }

  // Hash password and create user
  std::string hashedPassword = hashPassword(password);
  auto userDoc = createUserDocument(email, hashedPassword);

  try {
    dbManager.insertResource(collection_name, userDoc);

    std::vector<bsoncxx::document::value> result;
    std::vector<std::pair<std::string, std::string>> query;
    query.push_back({"email", email});
    dbManager.findCollection(0, collection_name, query, result);

    if (result.empty()) {
      throw AuthException("Failed to retrieve created user");
    }

    auto userView = result[0].view();
    User newUser(email, hashedPassword, role);
    newUser.id = userView["_id"].get_oid().value.to_string();
    return generateJWT(newUser);
  } catch (const std::exception& e) {
    throw AuthException("Failed to create user: " + std::string(e.what()));
  }
}

// Login
std::string AuthService::loginUser(const std::string& email,
                                   const std::string& password) {
  auto user = findUserByEmail(email);
  if (!user) {
    throw InvalidCredentialsException();
  }

  if (!verifyPassword(password, user->passwordHash)) {
    throw InvalidCredentialsException();
  }

  return generateJWT(*user);
}

// JWT Operations
std::string AuthService::generateJWT(const User& user) {
  auto token = jwt::create()
                   .set_issuer("auth-service")
                   .set_type("JWS")
                   .set_issued_at(std::chrono::system_clock::now())
                   .set_expires_at(std::chrono::system_clock::now() +
                                   std::chrono::hours(JWT_EXPIRATION_HOURS))
                   .set_payload_claim("userId", jwt::claim(user.id))
                   .set_payload_claim("email", jwt::claim(user.email))
                   .set_payload_claim("role", jwt::claim(user.role))
                   .sign(jwt::algorithm::hs256{JWT_SECRET});

  return token;
}

bool AuthService::verifyJWT(const std::string& token) {
  try {
    auto decoded = jwt::decode(token);
    auto verifier = jwt::verify()
                        .allow_algorithm(jwt::algorithm::hs256{JWT_SECRET})
                        .with_issuer("auth-service");

    verifier.verify(decoded);
    return true;
  } catch (const std::exception&) {
    return false;
  }
}

std::optional<JWTPayload> AuthService::decodeJWT(const std::string& token) {
  try {
    auto decoded = jwt::decode(token);
    JWTPayload payload;
    payload.userId = decoded.get_payload_claim("userId").as_string();
    payload.email = decoded.get_payload_claim("email").as_string();
    payload.role = decoded.get_payload_claim("role").as_string();
    payload.exp = std::chrono::duration_cast<std::chrono::seconds>(
                      decoded.get_expires_at().time_since_epoch())
                      .count();
    return payload;
  } catch (const std::exception&) {
    return std::nullopt;
  }
}

// Password Operations
std::string AuthService::hashPassword(const std::string& password) {
  return BCrypt::generateHash(password);
}

bool AuthService::verifyPassword(const std::string& password,
                                 const std::string& hash) {
  return BCrypt::validatePassword(password, hash);
}

// User Operations
std::optional<User> AuthService::findUserByEmail(const std::string& email) {
  std::vector<std::pair<std::string, std::string>> query;
  query.push_back({"email", email});

  std::vector<bsoncxx::document::value> result;
  dbManager.findCollection(0, collection_name, query, result);

  if (result.empty()) {
    return std::nullopt;
  }

  auto userDoc = result[0].view();
  User user;
  user.id = userDoc["_id"].get_oid().value.to_string();
  user.email = userDoc["email"].get_utf8().value.to_string();
  user.passwordHash = userDoc["passwordHash"].get_utf8().value.to_string();
  user.role = userDoc["role"].get_utf8().value.to_string();
  user.createdAt = userDoc["createdAt"].get_utf8().value.to_string();

  return user;
}

// Validation Methods
bool AuthService::isValidEmail(const std::string& email) {
  const std::regex pattern(
      R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
  return std::regex_match(email, pattern);
}

bool AuthService::isValidPassword(const std::string& password) {
  // At least 8 characters, 1 uppercase, 1 lowercase, 1 number
  const std::regex pattern(R"(^(?=.*[a-z])(?=.*[A-Z])(?=.*\d)[a-zA-Z\d]{8,}$)");
  return std::regex_match(password, pattern);
}

// Utility Methods
int64_t AuthService::getCurrentTimestamp() {
  return std::chrono::duration_cast<std::chrono::seconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

int64_t AuthService::getExpirationTimestamp() {
  return getCurrentTimestamp() + (JWT_EXPIRATION_HOURS * 3600);
}

std::vector<std::pair<std::string, std::string>>
AuthService::createUserDocument(const std::string& email,
                                const std::string& passwordHash,
                                const std::string& role) {
  std::vector<std::pair<std::string, std::string>> content;
  content.push_back({"email", email});
  content.push_back({"passwordHash", passwordHash});
  content.push_back({"role", role});
  content.push_back({"createdAt", std::to_string(getCurrentTimestamp())});
  return content;
}

// Middleware Implementation
bool authenticateToken(const crow::request& req) {
  auto authHeader = req.get_header_value("Authorization");
  if (authHeader.empty()) {
    return false;
  }

  std::string token = extractToken(authHeader);
  if (token.empty()) {
    return false;
  }

  static AuthService& authService =
      AuthService::getInstance();  // Assuming singleton pattern
  return authService.verifyJWT(token);
}

bool authorizeRole(const crow::request& req, const std::string& requiredRole) {
  auto authHeader = req.get_header_value("Authorization");
  if (authHeader.empty()) {
    return false;
  }

  std::string token = extractToken(authHeader);
  if (token.empty()) {
    return false;
  }

  static AuthService& authService = AuthService::getInstance();
  return authService.hasRole(token, requiredRole);
}

std::string extractToken(const std::string& authHeader) {
  if (authHeader.substr(0, 7) == "Bearer ") {
    return authHeader.substr(7);
  }
  return "";
}

// hasRole implementation
bool AuthService::hasRole(const std::string& token,
                          const std::string& requiredRole) {
  auto payload = decodeJWT(token);
  if (!payload) {
    return false;
  }
  return payload->role == requiredRole;
}
