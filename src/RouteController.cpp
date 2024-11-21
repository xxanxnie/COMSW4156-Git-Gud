// Copyright 2024 COMSW4156-Git-Gud

#include "RouteController.h"

#include <bsoncxx/json.hpp>
#include <exception>
#include <iostream>
#include <map>
#include <string>

#include "Food.h"
#include "Healthcare.h"
#include "Outreach.h"

crow::response handleException(const std::exception& e) {
  std::cerr << "Error: " << e.what() << std::endl;
  return crow::response{500, "An error has occurred: " + std::string(e.what())};
  std::cerr << "Error: " << e.what() << std::endl;
  return crow::response{500, "An error has occurred: " + std::string(e.what())};
}

// Temporary hardcoded API keys and their roles
std::map<std::string, std::string> validKeys = {
    // POST/PATCH/DELETE users
    {"abc123NGO", "NGO"},  // Non-profit Organizations
    {"def456VOL", "VOL"},  // Volunteers
    {"ghi789CLN", "CLN"},  // Clinics
    {"jkl012GOV", "GOV"},  // Government

    // GET users
    {"hml345HML", "HML"},  // Homeless
    {"rfg678RFG", "RFG"},  // Refugees
    {"vet901VET", "VET"},  // Veterans
    {"sub234SUB", "SUB"}   // Substance Users
};

bool authenticate(const crow::request& req, const std::string& requiredRole) {
  auto apiKey = req.get_header_value("API-Key");

  if (validKeys.find(apiKey) != validKeys.end()) {
    if (validKeys[apiKey] == requiredRole) {
      return true;
    }
  }

  return false;
}

bool authenticatePermissionsToGetAll(const crow::request& req) {
  return authenticate(req, "HML") || authenticate(req, "RFG") ||
         authenticate(req, "VET") || authenticate(req, "SUB");
}

bool authenticatePermissionsToPost(const crow::request& req) {
  return authenticate(req, "NGO") || authenticate(req, "VOL") ||
         authenticate(req, "CLN") || authenticate(req, "GOV");
}

// Add this helper method to RouteController class
bool RouteController::authenticateToken(const crow::request& req, crow::response& res) {
    auto authHeader = req.get_header_value("Authorization");
    if (authHeader.empty()) {
        res.code = 401;
        res.write("Authentication required. Please provide a valid token.");
        res.end();
        return false;
    }

    std::string token = extractToken(authHeader);
    if (token.empty()) {
        res.code = 401;
        res.write("Invalid authorization header format.");
        res.end();
        return false;
    }

    AuthService& authService = AuthService::getInstance();
    if (!authService.verifyJWT(token)) {
        res.code = 401;
        res.write("Invalid or expired token.");
        res.end();
        return false;
    }

    return true;
}

/**
 * Redirects to the homepage.
 *
 * @return A string containing the name of the html file to be loaded.
 */
void RouteController::index(crow::response& res) {
  res.write(
      "Welcome to the GitGud API. Use the appropriate endpoints to access "
      "resources.");
  res.end();
  res.write(
      "Welcome to the GitGud API. Use the appropriate endpoints to access "
      "resources.");
  res.end();
}

/**
 * Get all the shelter information in our database
 * GET with this key in json format
 * @return A crow::response object containing an HTTP 200 response,
 */
void RouteController::getShelter(const crow::request& req,
                                 crow::response& res) {
  if (!authenticateToken(req, res)) {
    return;
  }

  try {
    std::string response = shelterManager.searchShelterAll();
    res.code = 200;
    res.write(response);
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
  }
}
/**
 * Add the shelter information to our database
 * POST with this key in json format
 * @param ORG     The organization who gave the infomation.
 * @param User     The target this service provide for.
 * @param location     The location of the shelter.
 * @param capacity     The maximum number of users that the shelter can handle.
 * @param curUse       The current users that using this shelter.
 * @return A crow::response object containing an HTTP 201 response ,
 */
void RouteController::addShelter(const crow::request& req,
                                 crow::response& res) {
  if (!authenticateToken(req, res)) {
    return;
  }

  try {
    std::string result = shelterManager.addShelter(req.body);
    if (result.find("Error") != std::string::npos) {
      res.code = 400;
      res.write(result);
    } else {
      res.code = 201;
      res.write(result);
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
  }
}
/**
 * Update the shelter information to our database
 * POST with this key in json format
 * @param ORG     The organization who gave the infomation.
 * @param User     The target this service provide for.
 * @param location     The location of the shelter.
 * @param capacity     The maximum number of users that the shelter can handle.
 * @param curUse       The current users that using this shelter.
 * @return A crow::response object containing an HTTP 201 response ,
 */
void RouteController::updateShelter(const crow::request& req,
                                    crow::response& res) {
  if (!authenticateToken(req, res)) {
    return;
  }

  try {
    std::string result = shelterManager.updateShelter(req.body);
    if (result.find("Error") != std::string::npos) {
      res.code = 400;
      res.write(result);
    } else {
      res.code = 200;
      res.write("Shelter resource update successfully.");
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    res.end();
  }
}
/**
 * Delete the shelter information in our database
 * Delete with this key in json format
 * @param id     The organization who gave the infomation.
 * @return A crow::response object containing an HTTP 201 response ,
 */
void RouteController::deleteShelter(const crow::request& req,
                                    crow::response& res) {
  if (!authenticateToken(req, res)) {
    return;
  }

  try {
    auto resource = bsoncxx::from_json(req.body);
    std::vector<std::string> content;
    std::string id = "";
    for (auto element : resource.view()) {
      if (element.key().to_string() == "id") {
        id = element.get_utf8().value.to_string();
      }
    }
    shelterManager.deleteShelter(id);
    res.code = 200;
    res.write("Shelter resource deleted successfully.");
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    res.end();
  }
}

/**
 * Retrieves all counseling resources from the database.
 * GET request to fetch all counselors.
 *
 * @param req The incoming HTTP request (unused in this method).
 * @param res The HTTP response object to be sent back.
 * @return void. The method modifies the res object directly.
 */
void RouteController::getCounseling(const crow::request& req,
                                    crow::response& res) {
  if (!authenticateToken(req, res)) {
    return;
  }

  try {
    std::string response = counselingManager.searchCounselorsAll();
    res.code = 200;
    res.write(response);
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
  }
}

/**
 * Adds a new counseling resource to the database.
 * POST request with counselor information in JSON format.
 *
 * @param req The incoming HTTP request containing the counselor data.
 * @param res The HTTP response object to be sent back.
 * @return void. The method modifies the res object directly.
 *
 * Expected JSON format:
 * {
 *   "counselorName": "String",
 *   "specialty": "String"
 * }
 */

void RouteController::addCounseling(const crow::request& req,
                                    crow::response& res) {
  if (!authenticateToken(req, res)) {
    return;
  }

  try {
    std::string result = counselingManager.addCounselor(req.body);

    if (result.find("Error") != std::string::npos) {
      res.code = 400;
      res.write(result);
    } else {
      res.code = 201;
      res.write(result);
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    res.end();
  }
}

/**
 * Update the counseling information in our database
 * PATCH with this key in json format
 * @param id              The unique identifier of the counselor.
 * @param counselorName   The name of the counselor.
 * @param specialty       The specialty or expertise of the counselor.
 * @return A crow::response object containing an HTTP 200 response,
 */
void RouteController::updateCounseling(const crow::request& req,
                                       crow::response& res) {
  if (!authenticateToken(req, res)) {
    return;
  }

  try {
    std::string result = counselingManager.updateCounselor(req.body);
    if (result.find("Error") != std::string::npos) {
      res.code = 400;
      res.write(result);
    } else {
      res.code = 200;
      res.write("Counseling resource update successfully.");
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    res.end();
  }
}

/**
 * Delete the counseling information in our database
 * DELETE with this key in json format
 * @param id     The unique identifier of the counselor.
 * @return A crow::response object containing an HTTP 200 response,
 */
void RouteController::deleteCounseling(const crow::request& req,
                                       crow::response& res) {
  if (!authenticateToken(req, res)) {
    return;
  }

  try {
    auto resource = bsoncxx::from_json(req.body);
    std::string id = "";
    for (auto element : resource.view()) {
      if (element.key().to_string() == "id") {
        id = element.get_utf8().value.to_string();
      }
    }

    std::string result = counselingManager.deleteCounselor(id);

    if (result == "Success") {
      res.code = 200;
      res.write("Counseling resource deleted successfully.");
    } else {
      res.code = 400;
      res.write(result);
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    res.end();
  }
}

/**
 * @brief Adds a food resource to the database.
 *
 * This method processes a POST request to add a food resource. It parses the
 * request body as a JSON document, extracts the key-value pairs, and uses the
 * `Food` class to store the resource in the database.
 *
 * @param req The HTTP request containing the food resource in JSON format.
 * @param res The HTTP response object to send back to the client.
 *
 * @exception std::exception Throws if any error occurs during the database
 * interaction or JSON parsing.
 */
void RouteController::addFood(const crow::request& req, crow::response& res) {
  if (!authenticateToken(req, res)) {
    return;
  }

  try {
    std::string result = foodManager.addFood(req.body);
    if (result.find("Error") != std::string::npos) {
      res.code = 400;
      res.write(result);
    } else {
      res.code = 201;
      res.write(result);
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    res.end();
  }
}

/**
 * @brief Retrieves all food resources from the database.
 *
 * This method processes a GET request to fetch all food resources stored in the
 * database. It interacts with the `Food` class to retrieve the resources as a
 * JSON string, which is returned to the client in the response body.
 *
 * @param req The HTTP request. It does not require any input parameters in this
 * case.
 * @param res The HTTP response object used to send the data back to the client.
 *
 * @exception std::exception Throws if any error occurs during database
 * interaction or response handling.
 */
void RouteController::getAllFood(const crow::request& req,
                                 crow::response& res) {
  if (!authenticateToken(req, res)) {
    return;
  }

  try {
    // Get the response directly from the food manager
    std::string response = foodManager.getAllFood();

    // Return the raw response without additional formatting
    res.code = 200;
    res.write(response);
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
  }
}

/**
 * @brief Deletes a food resource from the database.
 *
 * This method processes a DELETE request to remove a food resource identified
 * by its ID. It interacts with the `Food` class to delete the resource from the
 * database.
 *
 * @param req The HTTP request containing the food resource ID in JSON format.
 * @param res The HTTP response object to send back to the client.
 *
 * @exception std::exception Throws if any error occurs during the database
 * interaction or JSON parsing.
 */
void RouteController::deleteFood(const crow::request& req,
                                 crow::response& res) {
  if (!authenticateToken(req, res)) {
    return;
  }

  try {
    // Input validation
    if (req.body.empty()) {
      res.code = 400;
      res.write("Invalid input: Request body cannot be empty.");
      res.end();
      return;
    }

    auto resource = bsoncxx::from_json(req.body);

    // Validate ID
    if (!resource["id"]) {
      res.code = 400;
      res.write("Invalid input: 'id' field is required");
      res.end();
      return;
    }

    std::string id = resource["id"].get_utf8().value.to_string();
    if (id.empty()) {
      res.code = 400;
      res.write("Invalid input: 'id' cannot be empty");
      res.end();
      return;
    }

    std::string result = foodManager.deleteFood(id);
    res.code = 200;
    res.write("Food resource deleted successfully.");
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    res.end();
  }
}

/**
 * @brief Updates a food resource in the database.
 *
 * This method processes a PATCH request to update a food resource. It validates
 * the required fields in the request body and updates only the provided fields.
 * Required fields in JSON format:
 * - id: The ID of the food resource to update
 * - At least one of: FoodType, Provider, location, quantity, expirationDate
 *
 * @param req The HTTP request containing the food resource ID and fields to
 * update.
 * @param res The HTTP response object to send back to the client.
 */
void RouteController::updateFood(const crow::request& req,
                                 crow::response& res) {
  if (!authenticateToken(req, res)) {
    return;
  }
  try {
    std::string result = foodManager.updateFood(req.body);
    if (result.find("Error") != std::string::npos) {
      res.code = 400;
      res.write(result);
    } else {
      res.code = 200;
      res.write("Food resource update successfully.");
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    res.end();
  }
}

/**
 * @brief Adds a new outreach service resource.
 *
 * This method extracts the outreach service details from the incoming
 * HTTP request, processes the data, and adds it to the database using
 * the Outreach class. It constructs the response based on
 * the outcome of the operation.
 *
 * @param req The HTTP request containing the outreach service data in JSON
 * format.
 * @param res The HTTP response that will be sent back to the client.
 *
 * @throws std::runtime_error if there is not enough data to add the outreach
 * service.
 */
void RouteController::addOutreachService(const crow::request& req,
                                         crow::response& res) {
  if (!authenticateToken(req, res)) {
    return;
  }

  try {
    std::string result = outreachManager.addOutreachService(req.body);
    if (result.find("Error") != std::string::npos) {
      res.code = 400;
      res.write(result);
    } else {
      res.code = 201;
      res.write(result);
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);  // Custom exception handling
  }
}

/**
 * @brief Retrieves all outreach services.
 *
 * This method processes an incoming HTTP request to fetch all outreach
 * services from the database. It sends the retrieved services back
 * in the HTTP response.
 *
 * @param req The HTTP request for retrieving outreach services.
 * @param res The HTTP response that will be sent back to the client,
 *             containing the outreach services in JSON format.
 */
void RouteController::getAllOutreachServices(const crow::request& req,
                                             crow::response& res) {
  if (!authenticateToken(req, res)) {
    return;
  }

  try {
    std::string response =
        outreachManager
            .getAllOutreachServices();  // Retrieve all outreach services
    res.code = 200;
    res.write(response);
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);  // Custom exception handling
  }
}

void RouteController::updateOutreach(const crow::request& req,
                                     crow::response& res) {
  if (!authenticateToken(req, res)) {
    return;
  }

  try {
    std::string result = outreachManager.updateOutreach(req.body);
    if (result.find("Error") != std::string::npos) {
      res.code = 400;
      res.write(result);
    } else {
      res.code = 200;
      res.write("Outreach resource update successfully.");
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    res.end();
  }
}

void RouteController::deleteOutreach(const crow::request& req,
                                     crow::response& res) {
  if (!authenticateToken(req, res)) {
    return;
  }

  try {
    auto resource = bsoncxx::from_json(req.body);
    std::vector<std::string> content;
    std::string id = "";
    for (auto element : resource.view()) {
      if (element.key().to_string() == "id") {
        id = element.get_utf8().value.to_string();
      }
    }
    outreachManager.deleteOutreach(id);
    res.code = 201;
    res.write("Outreach resource deleted successfully.");
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    res.end();
  }
}

/**
 * @brief Adds a new healthcare service.
 *
 * This function parses the incoming request body, extracts healthcare service
 * details, and invokes the HealthcareService manager to add the service to the
 * database. If successful, it sends a 201 HTTP response code along with a
 * success message. If an exception occurs, the error is handled and returned as
 * a 500 HTTP response.
 *
 * @param req The incoming HTTP request containing the healthcare service data
 * in JSON format.
 * @param res The HTTP response object to be sent back to the client.
 */
void RouteController::addHealthcareService(const crow::request& req,
                                           crow::response& res) {
  if (!authenticateToken(req, res)) {
    return;
  }

  try {
    std::string result = healthcareManager.addHealthcareService(req.body);
    if (result.find("Error") != std::string::npos) {
      res.code = 400;
      res.write(result);
    } else {
      res.code = 201;
      res.write(result);
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    res.end();
  }
}

/**
 * @brief Retrieves all healthcare services.
 *
 * This function invokes the Healthcare manager to fetch all healthcare services
 * from the database. The response is sent back as a 200 HTTP response with the
 * list of healthcare services in JSON format. If an exception occurs, the error
 * is handled and returned as a 500 HTTP response.
 *
 * @param req The incoming HTTP request.
 * @param res The HTTP response object to be sent back to the client containing
 * the list of services.
 */
void RouteController::getAllHealthcareServices(const crow::request& req,
                                               crow::response& res) {
  if (!authenticateToken(req, res)) {
    return;
  }

  try {
    std::string response = healthcareManager.getAllHealthcareServices();
    res.code = 200;
    res.write(response);
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
  }
}

void RouteController::updateHealthcareService(const crow::request& req,
                                              crow::response& res) {
  if (!authenticateToken(req, res)) {
    return;
  }

  try {
    std::string result = healthcareManager.updateHealthcare(req.body);
    if (result.find("Error") != std::string::npos) {
      res.code = 400;
      res.write(result);
    } else {
      res.code = 200;
      res.write("Healthcare resource update successfully.");
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    res.end();
  }
}

void RouteController::deleteHealthcareService(const crow::request& req,
                                              crow::response& res) {
  if (!authenticateToken(req, res)) {
    return;
  }

  try {
    auto resource = bsoncxx::from_json(req.body);
    std::string id = "";

    for (auto element : resource.view()) {
      if (element.key().to_string() == "id") {
        id = element.get_utf8().value.to_string();
      }
    }

    std::string msg = healthcareManager.deleteHealthcare(id);
    res.code = 201;
    res.write(msg);
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    res.end();
  }
}

void RouteController::registerUser(const crow::request& req, crow::response& res) {
    try {
        auto resource = bsoncxx::from_json(req.body);
        
        // Validate required fields
        if (!resource["email"] || !resource["password"]) {
            res.code = 400;
            res.write("Missing required fields: email and password");
            res.end();
            return;
        }

        std::string email = resource["email"].get_utf8().value.to_string();
        std::string password = resource["password"].get_utf8().value.to_string();

        AuthService& authService = AuthService::getInstance();
        std::string token = authService.registerUser(email, password);

        res.code = 201;
        res.write(token);
        res.end();
    } catch (const UserAlreadyExistsException& e) {
        res.code = 409;
        res.write(e.what());
        res.end();
    } catch (const AuthException& e) {
        res.code = 400;
        res.write(e.what());
        res.end();
    } catch (const std::exception& e) {
        res = handleException(e);
    }
}

void RouteController::loginUser(const crow::request& req, crow::response& res) {
    try {
        auto resource = bsoncxx::from_json(req.body);
        
        // Validate required fields
        if (!resource["email"] || !resource["password"]) {
            res.code = 400;
            res.write("Missing required fields: email and password");
            res.end();
            return;
        }

        std::string email = resource["email"].get_utf8().value.to_string();
        std::string password = resource["password"].get_utf8().value.to_string();

        AuthService& authService = AuthService::getInstance();
        std::string token = authService.loginUser(email, password);

        res.code = 200;
        res.write(token);
        res.end();
    } catch (const InvalidCredentialsException& e) {
        res.code = 401;
        res.write(e.what());
        res.end();
    } catch (const AuthException& e) {
        res.code = 400;
        res.write(e.what());
        res.end();
    } catch (const std::exception& e) {
        res = handleException(e);
    }
}

void RouteController::initRoutes(crow::SimpleApp& app) {
  CROW_ROUTE(app, "/").methods(crow::HTTPMethod::GET)(
      [this](const crow::request& req, crow::response& res) { index(res); });

  CROW_ROUTE(app, "/resources/food/add")
      .methods(crow::HTTPMethod::POST)(
          [this](const crow::request& req, crow::response& res) {
            addFood(req, res);
          });

  CROW_ROUTE(app, "/resources/food/getAll")
      .methods(crow::HTTPMethod::GET)(
          [this](const crow::request& req, crow::response& res) {
            getAllFood(req, res);
          });

  CROW_ROUTE(app, "/resources/food/delete")
      .methods(crow::HTTPMethod::DELETE)(
          [this](const crow::request& req, crow::response& res) {
            deleteFood(req, res);
          });

  CROW_ROUTE(app, "/resources/food/update")
      .methods(crow::HTTPMethod::PATCH)(
          [this](const crow::request& req, crow::response& res) {
            updateFood(req, res);
          });

  CROW_ROUTE(app, "/resources/shelter/add")
      .methods(crow::HTTPMethod::POST)(
          [this](const crow::request& req, crow::response& res) {
            addShelter(req, res);
          });
  CROW_ROUTE(app, "/resources/shelter/update")
      .methods(crow::HTTPMethod::PATCH)(
          [this](const crow::request& req, crow::response& res) {
            updateShelter(req, res);
          });
  CROW_ROUTE(app, "/resources/shelter/delete")
      .methods(crow::HTTPMethod::DELETE)(
          [this](const crow::request& req, crow::response& res) {
            deleteShelter(req, res);
          });
  CROW_ROUTE(app, "/resources/shelter/getAll")
      .methods(crow::HTTPMethod::GET)(
          [this](const crow::request& req, crow::response& res) {
            getShelter(req, res);
          });

  CROW_ROUTE(app, "/resources/counseling/getAll")
      .methods(crow::HTTPMethod::GET)(
          [this](const crow::request& req, crow::response& res) {
            getCounseling(req, res);
          });

  CROW_ROUTE(app, "/resources/counseling/add")
      .methods(crow::HTTPMethod::POST)(
          [this](const crow::request& req, crow::response& res) {
            addCounseling(req, res);
          });

  CROW_ROUTE(app, "/resources/counseling/update")
      .methods(crow::HTTPMethod::PATCH)(
          [this](const crow::request& req, crow::response& res) {
            updateCounseling(req, res);
          });

  CROW_ROUTE(app, "/resources/counseling/delete")
      .methods(crow::HTTPMethod::DELETE)(
          [this](const crow::request& req, crow::response& res) {
            deleteCounseling(req, res);
          });

  CROW_ROUTE(app, "/resources/outreach/add")
      .methods(crow::HTTPMethod::POST)(
          [this](const crow::request& req, crow::response& res) {
            addOutreachService(req, res);
          });

  CROW_ROUTE(app, "/resources/outreach/update")
      .methods(crow::HTTPMethod::PATCH)(
          [this](const crow::request& req, crow::response& res) {
            updateOutreach(req, res);
          });
  CROW_ROUTE(app, "/resources/outreach/delete")
      .methods(crow::HTTPMethod::DELETE)(
          [this](const crow::request& req, crow::response& res) {
            deleteOutreach(req, res);
          });

  CROW_ROUTE(app, "/resources/outreach/getAll")
      .methods(crow::HTTPMethod::GET)(
          [this](const crow::request& req, crow::response& res) {
            getAllOutreachServices(req, res);
          });

  CROW_ROUTE(app, "/resources/healthcare/add")
      .methods(crow::HTTPMethod::POST)(
          [this](const crow::request& req, crow::response& res) {
            addHealthcareService(req, res);
          });

  CROW_ROUTE(app, "/resources/healthcare/getAll")
      .methods(crow::HTTPMethod::GET)(
          [this](const crow::request& req, crow::response& res) {
            getAllHealthcareServices(req, res);
          });

  CROW_ROUTE(app, "/resources/healthcare/update")
      .methods(crow::HTTPMethod::PATCH)(
          [this](const crow::request& req, crow::response& res) {
            updateHealthcareService(req, res);
          });

  CROW_ROUTE(app, "/resources/healthcare/delete")
      .methods(crow::HTTPMethod::DELETE)(
          [this](const crow::request& req, crow::response& res) {
            deleteHealthcareService(req, res);
          });

  CROW_ROUTE(app, "/auth/register")
      .methods(crow::HTTPMethod::POST)
      ([this](const crow::request& req, crow::response& res) {
          registerUser(req, res);
      });

  CROW_ROUTE(app, "/auth/login")
      .methods(crow::HTTPMethod::POST)
      ([this](const crow::request& req, crow::response& res) {
          loginUser(req, res);
      });
}
