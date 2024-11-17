#ifndef AUTH_H
#define AUTH_H

#include <string>
#include <memory>
#include <stdexcept>
#include <optional>
#include "DatabaseManager.h"
#include "../external_libraries/Crow/include/crow.h"

// Custom exceptions for authentication
class AuthException : public std::runtime_error {
public:
    explicit AuthException(const std::string& message) : std::runtime_error(message) {}
};

class InvalidCredentialsException : public AuthException {
public:
    InvalidCredentialsException() : AuthException("Invalid email or password") {}
};

class UserAlreadyExistsException : public AuthException {
public:
    UserAlreadyExistsException() : AuthException("User with this email already exists") {}
};

// User model
struct User {
    std::string id;
    std::string email;
    std::string passwordHash;
    std::string role;
    std::string createdAt;

    // Updated constructor
    User(const std::string& email, 
         const std::string& passwordHash,
         const std::string& role = "user")
        : email(email)
        , passwordHash(passwordHash)
        , role(role) {}
    
    // Default constructor
    User() = default;
};

// JWT payload structure
struct JWTPayload {
    std::string userId;
    std::string email;
    std::string role;
    int64_t exp;  // Expiration time
};

class AuthService {
private:
    // Private constructor for singleton pattern
    AuthService();
    
    // Delete copy constructor and assignment operator
    AuthService(const AuthService&) = delete;
    AuthService& operator=(const AuthService&) = delete;

public:
    // Static method to get the singleton instance
    static AuthService& getInstance() {
        static AuthService instance;
        return instance;
    }
    
    explicit AuthService(DatabaseManager& dbManager);
    ~AuthService() = default;

    // User registration and login
    std::string registerUser(const std::string& email, 
                           const std::string& password);
    
    std::string loginUser(const std::string& email, 
                         const std::string& password);
    
    // JWT operations
    std::string generateJWT(const User& user);
    bool verifyJWT(const std::string& token);
    std::optional<JWTPayload> decodeJWT(const std::string& token);

    // User operations
    std::optional<User> findUserByEmail(const std::string& email);
    bool updateUserPassword(const std::string& userId, const std::string& newPassword);
    bool deleteUser(const std::string& userId);

    // Role-based authorization
    bool hasRole(const std::string& token, const std::string& requiredRole);
    
private:
    DatabaseManager& dbManager;
    const std::string collection_name = "Users";
    const int JWT_EXPIRATION_HOURS = 24;
    const std::string JWT_SECRET = "your-secret-key";  // In production, load from env variables

    // Password hashing
    std::string hashPassword(const std::string& password);
    bool verifyPassword(const std::string& password, const std::string& hash);

    // JWT utilities
    int64_t getCurrentTimestamp();
    int64_t getExpirationTimestamp();
    
    // User validation
    bool isValidEmail(const std::string& email);
    bool isValidPassword(const std::string& password);

    // Database operations
    std::vector<std::pair<std::string, std::string>> createUserDocument(
        const std::string& email,
        const std::string& passwordHash,
        const std::string& role = "user"
    );
};

// Middleware function for token verification
bool authenticateToken(const crow::request& req);

// Middleware function for role-based authorization
bool authorizeRole(const crow::request& req, const std::string& requiredRole);

// Helper function to extract token from Authorization header
std::string extractToken(const std::string& authHeader);

#endif // AUTH_H
