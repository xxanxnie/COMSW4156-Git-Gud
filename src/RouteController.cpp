// Copyright 2024 COMSW4156-Git-Gud

#include "RouteController.h"

#include <bsoncxx/json.hpp>
#include <exception>
#include <iostream>
#include <map>
#include <string>

#include "Food.h"
#include "Healthcare.h"
#include "Logger.h"
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
bool RouteController::authenticateToken(const crow::request& req,
                                        crow::response& res) {
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
  LOG_INFO("RouteController", "index endpoint called");
  std::string message =
      "Welcome to the GitGud API. Use the appropriate endpoints to access "
      "resources.";
  res.write(message);
  LOG_INFO("RouteController", "index response: code={}, message={}", 200,
           message);
  res.end();
}

/**
 * @brief Handles the HTTP GET request to fetch all shelter data.
 *
 * This method is responsible for processing a GET request that retrieves
 * shelter data from the shelterManager. It handles authentication, pagination
 * (via the "start" parameter), and returns the shelter data in a JSON format.
 *
 * @param req The HTTP request object containing the request data.
 * @param res The HTTP response object used to send the response.
 *
 * @throws std::exception If there is an error during data retrieval or response
 * processing.
 * @return void This method directly modifies the `res` parameter to send the
 * response.
 */
void RouteController::getShelter(const crow::request& req,
                                 crow::response& res) {
  LOG_INFO("RouteController", "getShelter called with URL: {}", req.url);
  if (!authenticateToken(req, res)) {
    LOG_ERROR("RouteController", "Authentication failed in getShelter");
    return;
  }

  try {
    int start = 0;
    auto start_param = req.url_params.get("start");
    if (start_param && std::stoi(start_param) > 0) {
      start = std::stoi(start_param);
    }
    std::string response = shelterManager.searchShelterAll(start);
    res.code = 200;
    res.write(response);
    LOG_INFO("RouteController", "getShelter response: code={}, body={}",
             res.code, response);
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController", "getShelter error: code={}, error={}",
              res.code, e.what());
    res.end();
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
  LOG_INFO("RouteController", "addShelter request body: {}", req.body);
  if (!authenticateToken(req, res)) {
    LOG_ERROR("RouteController", "Authentication failed in addShelter");
    return;
  }

  try {
    std::string result = shelterManager.addShelter(req.body);
    if (result.find("Error") != std::string::npos) {
      res.code = 400;
      res.write(result);
      LOG_ERROR("RouteController",
                "addShelter validation error: code={}, message={}", res.code,
                result);
    } else {
      res.code = 201;
      res.write(result);
      LOG_INFO("RouteController", "addShelter success: code={}, response={}",
               res.code, result);
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController", "addShelter error: code={}, error={}",
              res.code, e.what());
    res.end();
  }
}
/**
 * @brief Handles the HTTP POST request to add a new shelter.
 *
 * This method processes a POST request to add new shelter data. It first checks
 * if the user is authorized to add a shelter. If the user is authorized, it
 * attempts to add the shelter data provided in the request body to the
 * shelterManager. The result (either an ID or an error message) is then
 * returned to the client.
 *
 * @param req The HTTP request object containing the shelter data in its body.
 * @param res The HTTP response object used to send the response back to the
 * client.
 *
 * @throws std::exception If there is an error during the shelter addition
 * process or data handling.
 * @return void This method directly modifies the `res` parameter to send the
 * response.
 */
void RouteController::updateShelter(const crow::request& req,
                                    crow::response& res) {
  LOG_INFO("RouteController", "updateShelter request body: {}", req.body);
  if (!authenticateToken(req, res)) {
    LOG_ERROR("RouteController", "Authentication failed in updateShelter");
    return;
  }

  try {
    std::string result = shelterManager.updateShelter(req.body);
    if (result.find("Error") != std::string::npos) {
      res.code = 400;
      res.write(result);
      LOG_ERROR("RouteController",
                "updateShelter validation error: code={}, message={}", res.code,
                result);
    } else {
      res.code = 200;
      res.write("Shelter resource updated successfully.");
      LOG_INFO("RouteController", "updateShelter success: code={}, response={}",
               res.code, result);
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController", "updateShelter error: code={}, error={}",
              res.code, e.what());
    res.end();
  }
}
/**
 * @brief Handles the HTTP DELETE request to remove a shelter.
 *
 * This method processes a DELETE request to remove a shelter based on the
 * provided shelter ID in the request body. It first checks if the user has
 * permission to perform the delete operation. If authorized, it attempts to
 * delete the shelter with the specified ID. Upon success, it responds with a
 * success message. If an error occurs, an appropriate error message is returned
 * to the client.
 *
 * @param req The HTTP request object containing the shelter ID in its body for
 * deletion.
 * @param res The HTTP response object used to send the response back to the
 * client.
 *
 * @throws std::exception If there is an error during the deletion process or
 * data handling.
 * @return void This method directly modifies the `res` parameter to send the
 * response.
 */
void RouteController::deleteShelter(const crow::request& req,
                                    crow::response& res) {
  LOG_INFO("RouteController", "deleteShelter request body: {}", req.body);
  if (!authenticateToken(req, res)) {
    LOG_ERROR("RouteController", "Authentication failed in deleteShelter");
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
    shelterManager.deleteShelter(id);
    res.code = 200;
    std::string message = "Shelter resource deleted successfully.";
    res.write(message);
    LOG_INFO("RouteController", "deleteShelter success: code={}, id={}",
             res.code, id);
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController", "deleteShelter error: code={}, error={}",
              res.code, e.what());
    res.end();
  }
}

/**
 * @brief Handles the HTTP GET request to retrieve all counseling services.
 *
 * This method processes a GET request to fetch a list of all available
 * counseling services. It first checks if the user has permission to access
 * this information. If authorized, it retrieves the data from the counseling
 * manager and responds with the result. If an error occurs during the process,
 * an appropriate error message is sent back to the client.
 *
 * @param req The HTTP request object containing the parameters for fetching the
 * counseling services.
 * @param res The HTTP response object used to send the response back to the
 * client.
 *
 * @throws std::exception If an error occurs while fetching the counseling
 * services or processing the request.
 * @return void This method modifies the `res` parameter directly to send the
 * response to the client.
 */
void RouteController::getCounseling(const crow::request& req,
                                    crow::response& res) {
  LOG_INFO("RouteController", "getCounseling called with URL: {}", req.url);
  if (!authenticateToken(req, res)) {
    LOG_ERROR("RouteController", "Authentication failed in getCounseling");
    return;
  }

  try {
    int start = 0;
    auto start_param = req.url_params.get("start");
    if (start_param && std::stoi(start_param) > 0) {
      start = std::stoi(start_param);
    }
    std::string response = counselingManager.searchCounselorsAll(start);
    res.code = 200;
    res.write(response);
    LOG_INFO("RouteController", "getCounseling response: code={}, body={}",
             res.code, response);
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController", "getCounseling error: code={}, error={}",
              res.code, e.what());
    res.end();
  }
}

/**
 * @brief Handles the HTTP POST request to add a new counseling service.
 *
 * This method processes a POST request to add a new counseling service. It
 * first checks if the user has the necessary permissions to post new services.
 * If authorized, it attempts to add the counseling service through the
 * counseling manager. If successful, it returns a success message with the
 * created resource's ID. If an error occurs, an error message is returned.
 *
 * @param req The HTTP request object containing the details of the counseling
 * service to be added.
 * @param res The HTTP response object used to send the response back to the
 * client.
 *
 * @throws std::exception If an error occurs while adding the counseling service
 * or processing the request.
 * @return void This method modifies the `res` parameter directly to send the
 * response to the client.
 */

void RouteController::addCounseling(const crow::request& req,
                                    crow::response& res) {
  LOG_INFO("RouteController", "addCounseling request body: {}", req.body);
  if (!authenticateToken(req, res)) {
    LOG_ERROR("RouteController", "Authentication failed in addCounseling");
    return;
  }

  try {
    std::string result = counselingManager.addCounselor(req.body);
    if (result.find("Error") != std::string::npos) {
      res.code = 400;
      res.write(result);
      LOG_ERROR("RouteController",
                "addCounseling validation error: code={}, message={}", res.code,
                result);
    } else {
      res.code = 201;
      res.write(result);
      LOG_INFO("RouteController", "addCounseling success: code={}, response={}",
               res.code, result);
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController", "addCounseling error: code={}, error={}",
              res.code, e.what());
    res.end();
  }
}

/**
 * @brief Handles the HTTP PUT request to update an existing counseling service.
 *
 * This method processes a PUT request to update the details of an existing
 * counseling service. It first checks if the user has the necessary permissions
 * to update the service. If authorized, it attempts to update the counseling
 * service using the provided data. If the update is successful, a success
 * message is returned. If an error occurs, an error message is sent in the
 * response.
 *
 * @param req The HTTP request object containing the updated details of the
 * counseling service.
 * @param res The HTTP response object used to send the response back to the
 * client.
 *
 * @throws std::exception If an error occurs while updating the counseling
 * service or processing the request.
 * @return void This method modifies the `res` parameter directly to send the
 * response to the client.
 */
void RouteController::updateCounseling(const crow::request& req,
                                       crow::response& res) {
  LOG_INFO("RouteController", "updateCounseling request body: {}", req.body);
  if (!authenticateToken(req, res)) {
    LOG_ERROR("RouteController", "Authentication failed in updateCounseling");
    return;
  }

  try {
    std::string result = counselingManager.updateCounselor(req.body);
    if (result.find("Error") != std::string::npos) {
      res.code = 400;
      res.write(result);
      LOG_ERROR("RouteController",
                "updateCounseling validation error: code={}, message={}",
                res.code, result);
    } else {
      res.code = 200;
      res.write("Counseling resource updated successfully.");
      LOG_INFO("RouteController",
               "updateCounseling success: code={}, response={}", res.code,
               result);
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController", "updateCounseling error: code={}, error={}",
              res.code, e.what());
    res.end();
  }
}

/**
 * @brief Handles the HTTP DELETE request to delete an existing counseling
 * service.
 *
 * This method processes a DELETE request to remove a counseling service from
 * the database. It first checks if the user has the necessary permissions to
 * delete the service. If authorized, it attempts to delete the counseling
 * service identified by the `id` provided in the request body. If the deletion
 * is successful, a success message is returned. If there is an error, an error
 * message is sent in the response.
 *
 * @param req The HTTP request object containing the ID of the counseling
 * service to be deleted.
 * @param res The HTTP response object used to send the response back to the
 * client.
 *
 * @throws std::exception If an error occurs while deleting the counseling
 * service or processing the request.
 * @return void This method modifies the `res` parameter directly to send the
 * response to the client.
 */
void RouteController::deleteCounseling(const crow::request& req,
                                       crow::response& res) {
  LOG_INFO("RouteController", "deleteCounseling request body: {}", req.body);
  if (!authenticateToken(req, res)) {
    LOG_ERROR("RouteController", "Authentication failed in deleteCounseling");
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
    counselingManager.deleteCounselor(id);
    res.code = 200;
    std::string message = "Counseling resource deleted successfully.";
    res.write(message);
    LOG_INFO("RouteController", "deleteCounseling success: code={}, id={}",
             res.code, id);
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController", "deleteCounseling error: code={}, error={}",
              res.code, e.what());
    res.end();
  }
}

/**
 * @brief Handles the HTTP POST request to add a new food resource.
 *
 * This method processes a POST request to add a new food item to the system. It
 * first checks if the user has the necessary permissions to perform the
 * operation. If authorized, it attempts to add the food item using the data
 * provided in the request body. If the food item is successfully added, the
 * response will include the result, typically the ID of the newly created food
 * resource. If there is an error during the process, an error message is
 * returned instead.
 *
 * @param req The HTTP request object containing the data of the food item to be
 * added.
 * @param res The HTTP response object used to send the response back to the
 * client.
 *
 * @throws std::exception If an error occurs while adding the food item or
 * processing the request.
 * @return void This method modifies the `res` parameter directly to send the
 * response to the client.
 */
void RouteController::addFood(const crow::request& req, crow::response& res) {
  LOG_INFO("RouteController", "addFood request body: {}", req.body);
  if (!authenticateToken(req, res)) {
    LOG_ERROR("RouteController", "Authentication failed in addFood");
    return;
  }

  try {
    std::string result = foodManager.addFood(req.body);
    if (result.find("Error") != std::string::npos) {
      res.code = 400;
      res.write(result);
      LOG_ERROR("RouteController",
                "addFood validation error: code={}, message={}", res.code,
                result);
    } else {
      res.code = 201;
      res.write(result);
      LOG_INFO("RouteController", "addFood success: code={}, response={}",
               res.code, result);
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController", "addFood error: code={}, error={}", res.code,
              e.what());
    res.end();
  }
}

/**
 * @brief Handles the HTTP GET request to retrieve all food resources.
 *
 * This method processes a GET request to fetch all food items from the system.
 * It checks if the user has the necessary permissions to access the food
 * resource data. If authorized, it fetches the data starting from a specified
 * point (if provided) and returns the results in the response. The results are
 * returned as raw JSON without any additional formatting. If an error occurs
 * during the process, an appropriate error message is returned instead.
 *
 * @param req The HTTP request object containing the query parameters and other
 * necessary information.
 * @param res The HTTP response object used to send the response back to the
 * client.
 *
 * @throws std::exception If an error occurs while fetching the food data or
 * processing the request.
 * @return void This method modifies the `res` parameter directly to send the
 * response to the client.
 */
void RouteController::getAllFood(const crow::request& req,
                                 crow::response& res) {
  LOG_INFO("RouteController", "getAllFood called with URL: {}", req.url);
  if (!authenticateToken(req, res)) {
    LOG_ERROR("RouteController", "Authentication failed in getAllFood");
    return;
  }

  try {
    // Get the response directly from the food manager
    int start = 0;
    auto start_param = req.url_params.get("start");
    if (start_param && std::stoi(start_param) > 0) {
      start = std::stoi(start_param);
    }
    std::string response = foodManager.getAllFood(start);

    // Return the raw response without additional formatting
    res.code = 200;
    res.write(response);
    LOG_INFO("RouteController", "getAllFood response: code={}, body={}",
             res.code, response);
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController", "getAllFood error: code={}, error={}",
              res.code, e.what());
    res.end();
  }
}

/**
 * @brief Handles the HTTP DELETE request to remove a food resource.
 *
 * This method processes a DELETE request to remove a food item from the system.
 * It checks if the user has the necessary permissions to delete the resource.
 * It then validates the request body to ensure that the `id` field is provided
 * and not empty. If the validation passes, it proceeds to delete the food
 * resource with the given ID. If any issues arise (such as missing or invalid
 * data), the response will include an error message.
 *
 * @param req The HTTP request object containing the request body with the food
 * ID to be deleted.
 * @param res The HTTP response object used to send the response back to the
 * client.
 *
 * @throws std::exception If an error occurs during the validation or deletion
 * process.
 * @return void This method modifies the `res` parameter directly to send the
 * response to the client.
 */
void RouteController::deleteFood(const crow::request& req,
                                 crow::response& res) {
  LOG_INFO("RouteController", "deleteFood request body: {}", req.body);
  if (!authenticateToken(req, res)) {
    LOG_ERROR("RouteController", "Authentication failed in deleteFood");
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
    foodManager.deleteFood(id);
    res.code = 200;
    std::string message = "Food resource deleted successfully.";
    res.write(message);
    LOG_INFO("RouteController", "deleteFood success: code={}, id={}", res.code,
             id);
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController", "deleteFood error: code={}, error={}",
              res.code, e.what());
    res.end();
  }
}

/**
 * @brief Handles the HTTP PUT request to update a food resource.
 *
 * This method processes a PUT request to update the details of an existing food
 * resource in the system. It checks if the user has the necessary permissions
 * to update the resource. After validation, it attempts to update the food
 * resource using the provided request body. If successful, it responds with a
 * success message. If there is any error, it returns an error message. In case
 * of exceptions, the appropriate error response is sent.
 *
 * @param req The HTTP request object containing the request body with the food
 * resource details to be updated.
 * @param res The HTTP response object used to send the response back to the
 * client.
 *
 * @throws std::exception If an error occurs during the update process.
 * @return void This method modifies the `res` parameter directly to send the
 * response to the client.
 */
void RouteController::updateFood(const crow::request& req,
                                 crow::response& res) {
  LOG_INFO("RouteController", "updateFood request body: {}", req.body);
  if (!authenticateToken(req, res)) {
    LOG_ERROR("RouteController", "Authentication failed in updateFood");
    return;
  }

  try {
    std::string result = foodManager.updateFood(req.body);
    if (result.find("Error") != std::string::npos) {
      res.code = 400;
      res.write(result);
      LOG_ERROR("RouteController",
                "updateFood validation error: code={}, message={}", res.code,
                result);
    } else {
      res.code = 200;
      res.write("Food resource updated successfully.");
      LOG_INFO("RouteController", "updateFood success: code={}, response={}",
               res.code, result);
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController", "updateFood error: code={}, error={}",
              res.code, e.what());
    res.end();
  }
}

/**
 * @brief Handles the HTTP POST request to add a new outreach service.
 *
 * This method processes a POST request to add a new outreach service to the
 * system. It first checks if the user has the necessary permissions to create a
 * new service. If the user is authorized, it attempts to add the outreach
 * service using the data in the request body. If successful, it responds with
 * the ID of the newly created service. If there is an error, it responds with
 * an error message. The method also handles any exceptions that occur during
 * the process.
 *
 * @param req The HTTP request object containing the outreach service details to
 * be added.
 * @param res The HTTP response object used to send the response back to the
 * client.
 *
 * @throws std::exception If an error occurs during the process of adding the
 * outreach service.
 * @return void This method directly modifies the `res` object to send a
 * response to the client.
 */
void RouteController::addOutreachService(const crow::request& req,
                                         crow::response& res) {
  LOG_INFO("RouteController", "addOutreachService request body: {}", req.body);
  if (!authenticateToken(req, res)) {
    LOG_ERROR("RouteController", "Authentication failed in addOutreachService");
    return;
  }

  try {
    std::string result = outreachManager.addOutreachService(req.body);
    if (result.find("Error") != std::string::npos) {
      res.code = 400;
      res.write(result);
      LOG_ERROR("RouteController",
                "addOutreachService validation error: code={}, message={}",
                res.code, result);
    } else {
      res.code = 201;
      res.write(result);
      LOG_INFO("RouteController",
               "addOutreachService success: code={}, response={}", res.code,
               result);
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController", "addOutreachService error: code={}, error={}",
              res.code, e.what());
    res.end();
  }
}

/**
 * @brief Handles the HTTP GET request to retrieve all outreach services.
 *
 * This method processes a GET request to retrieve a list of all outreach
 * services starting from a specified index (if provided). It checks if the user
 * has the necessary permissions to view the outreach services. If the user is
 * authorized, it retrieves the outreach services and returns them in the
 * response. If an error occurs during the process, it handles the exception and
 * sends an error response.
 *
 * @param req The HTTP request object that may contain query parameters like
 * `start` to specify the starting index for pagination.
 * @param res The HTTP response object used to send the response back to the
 * client.
 *
 * @throws std::exception If an error occurs during the retrieval of outreach
 * services.
 * @return void This method directly modifies the `res` object to send a
 * response to the client.
 */
void RouteController::getAllOutreachServices(const crow::request& req,
                                             crow::response& res) {
  LOG_INFO("RouteController", "getAllOutreachServices called with URL: {}",
           req.url);
  if (!authenticateToken(req, res)) {
    LOG_ERROR("RouteController",
              "Authentication failed in getAllOutreachServices");
    return;
  }

  try {
    int start = 0;
    auto start_param = req.url_params.get("start");
    if (start_param && std::stoi(start_param) > 0) {
      start = std::stoi(start_param);
    }
    std::string response = outreachManager.getAllOutreachServices(
        start);  // Retrieve all outreach services
    res.code = 200;
    res.write(response);
    LOG_INFO("RouteController",
             "getAllOutreachServices response: code={}, body={}", res.code,
             response);
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController",
              "getAllOutreachServices error: code={}, error={}", res.code,
              e.what());
    res.end();
  }
}
/**
 * @brief Handles the HTTP POST request to update an existing outreach service.
 *
 * This method processes a POST request to update an outreach service. It checks
 * if the user has the necessary permissions to update the outreach service. If
 * authorized, it updates the resource using the provided data and sends an
 * appropriate response. If any error occurs during the process, the exception
 * is handled and an error message is returned.
 *
 * @param req The HTTP request object that contains the data for the outreach
 * service update.
 * @param res The HTTP response object used to send the response back to the
 * client.
 *
 * @throws std::exception If an error occurs during the update process.
 * @return void This method directly modifies the `res` object to send a
 * response to the client.
 */
void RouteController::updateOutreach(const crow::request& req,
                                     crow::response& res) {
  LOG_INFO("RouteController", "updateOutreach request body: {}", req.body);
  if (!authenticateToken(req, res)) {
    LOG_ERROR("RouteController", "Authentication failed in updateOutreach");
    return;
  }

  try {
    std::string result = outreachManager.updateOutreach(req.body);
    if (result.find("Error") != std::string::npos) {
      res.code = 400;
      res.write(result);
      LOG_ERROR("RouteController",
                "updateOutreach validation error: code={}, message={}",
                res.code, result);
    } else {
      res.code = 200;
      res.write("Outreach resource update successfully.");
      LOG_INFO("RouteController",
               "updateOutreach success: code={}, response={}", res.code,
               result);
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController", "updateOutreach error: code={}, error={}",
              res.code, e.what());
    res.end();
  }
}
/**
 * @brief Handles the HTTP request to delete an existing outreach service.
 *
 * This method processes a request to delete an outreach service identified by
 * its ID. It first checks if the user has the necessary permissions to perform
 * the delete operation. If authorized, the outreach service is deleted. If any
 * errors occur, appropriate error messages are returned.
 *
 * @param req The HTTP request object containing the ID of the outreach service
 * to be deleted.
 * @param res The HTTP response object used to send the result back to the
 * client.
 *
 * @throws std::exception If an error occurs during the deletion process.
 * @return void This method directly modifies the `res` object to send a
 * response to the client.
 */
void RouteController::deleteOutreach(const crow::request& req,
                                     crow::response& res) {
  LOG_INFO("RouteController", "deleteOutreach request body: {}", req.body);
  if (!authenticateToken(req, res)) {
    LOG_ERROR("RouteController", "Authentication failed in deleteOutreach");
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
    outreachManager.deleteOutreach(id);
    res.code = 200;
    std::string message = "Outreach resource deleted successfully.";
    res.write(message);
    LOG_INFO("RouteController", "deleteOutreach success: code={}, id={}",
             res.code, id);
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController", "deleteOutreach error: code={}, error={}",
              res.code, e.what());
    res.end();
  }
}

/**
 * @brief Handles the HTTP request to add a new healthcare service.
 *
 * This method processes a request to add a new healthcare service. It checks if
 * the user has the necessary permissions to add a service. If authorized, it
 * calls the healthcare manager to add the new service. Based on the result of
 * the addition, it responds with either a success or an error message.
 *
 * @param req The HTTP request object containing the data for the new healthcare
 * service.
 * @param res The HTTP response object used to send the result back to the
 * client.
 *
 * @throws std::exception If an error occurs during the addition process.
 * @return void This method directly modifies the `res` object to send a
 * response to the client.
 */
void RouteController::addHealthcareService(const crow::request& req,
                                           crow::response& res) {
  LOG_INFO("RouteController", "addHealthcareService request body: {}",
           req.body);
  if (!authenticateToken(req, res)) {
    LOG_ERROR("RouteController",
              "Authentication failed in addHealthcareService");
    return;
  }

  try {
    std::string result = healthcareManager.addHealthcareService(req.body);

    auto resource = bsoncxx::from_json(req.body);
    std::string city = "";

    for (auto element : resource.view()) {
      if (element.key().to_string() == "city")
        city = element.get_utf8().value.to_string();
    }

    if (result.find("Error") != std::string::npos) {
      res.code = 400;
      res.write(result);
      LOG_ERROR("RouteController",
                "addHealthcareService validation error: code={}, message={}",
                res.code, result);
    } else {
      subscriptionManager.notifySubscribers("healthcare", city);
      res.code = 201;
      res.write(result);
      LOG_INFO("RouteController",
               "addHealthcareService success: code={}, response={}", res.code,
               result);
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController",
              "addHealthcareService error: code={}, error={}", res.code,
              e.what());
    res.end();
  }
}

/**
 * @brief Handles the HTTP request to retrieve all healthcare services.
 *
 * This method processes a request to fetch a list of all healthcare services.
 * It checks if the user has the necessary permissions to view all healthcare
 * services. If authorized, it retrieves the services starting from a specified
 * index (default is 0) and sends the response to the client. In case of an
 * error, an appropriate error message is returned.
 *
 * @param req The HTTP request object containing any parameters, such as
 * pagination information.
 * @param res The HTTP response object used to send the result back to the
 * client.
 *
 * @throws std::exception If an error occurs during the retrieval of healthcare
 * services.
 * @return void This method directly modifies the `res` object to send a
 * response to the client.
 */
void RouteController::getAllHealthcareServices(const crow::request& req,
                                               crow::response& res) {
  LOG_INFO("RouteController", "getAllHealthcareServices called with URL: {}",
           req.url);
  if (!authenticateToken(req, res)) {
    LOG_ERROR("RouteController",
              "Authentication failed in getAllHealthcareServices");
    return;
  }

  try {
    int start = 0;
    auto start_param = req.url_params.get("start");
    if (start_param && std::stoi(start_param) > 0) {
      start = std::stoi(start_param);
    }
    std::string response = healthcareManager.getAllHealthcareServices(start);
    res.code = 200;
    res.write(response);
    LOG_INFO("RouteController",
             "getAllHealthcareServices response: code={}, body={}", res.code,
             response);
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController",
              "getAllHealthcareServices error: code={}, error={}", res.code,
              e.what());
    res.end();
  }
}
/**
 * @brief Handles the HTTP request to update a healthcare service resource.
 *
 * This method processes a request to update a healthcare service. It first
 * checks if the user has the necessary permissions to perform the update. If
 * authorized, it attempts to update the healthcare resource using the request
 * body. If the update is successful, it returns a success message, otherwise an
 * error message is returned. In case of an exception, an error response is
 * sent.
 *
 * @param req The HTTP request object containing the data necessary to update
 * the healthcare service.
 * @param res The HTTP response object used to send the result back to the
 * client.
 *
 * @throws std::exception If an error occurs during the update of the healthcare
 * service.
 * @return void This method directly modifies the `res` object to send a
 * response to the client.
 */
void RouteController::updateHealthcareService(const crow::request& req,
                                              crow::response& res) {
  LOG_INFO("RouteController", "updateHealthcareService request body: {}",
           req.body);
  if (!authenticateToken(req, res)) {
    LOG_ERROR("RouteController",
              "Authentication failed in updateHealthcareService");
    return;
  }

  try {
    std::string result = healthcareManager.updateHealthcare(req.body);
    if (result.find("Error") != std::string::npos) {
      res.code = 400;
      res.write(result);
      LOG_ERROR("RouteController",
                "updateHealthcareService validation error: code={}, message={}",
                res.code, result);
    } else {
      res.code = 200;
      res.write("Healthcare resource update successfully.");
      LOG_INFO("RouteController",
               "updateHealthcareService success: code={}, response={}",
               res.code, result);
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController",
              "updateHealthcareService error: code={}, error={}", res.code,
              e.what());
    res.end();
  }
}
/**
 * @brief Handles the HTTP request to delete a healthcare service resource.
 *
 * This method processes a request to delete a healthcare service resource. It
 * first checks if the user has the necessary permissions to perform the
 * deletion. If authorized, it retrieves the healthcare service ID from the
 * request body and attempts to delete the corresponding healthcare resource. A
 * success or error message is returned based on the outcome. In case of an
 * exception, an error response is sent.
 *
 * @param req The HTTP request object containing the data necessary to identify
 * and delete the healthcare service.
 * @param res The HTTP response object used to send the result back to the
 * client.
 *
 * @throws std::exception If an error occurs during the deletion of the
 * healthcare service.
 * @return void This method directly modifies the `res` object to send a
 * response to the client.
 */
void RouteController::deleteHealthcareService(const crow::request& req,
                                              crow::response& res) {
  LOG_INFO("RouteController", "deleteHealthcareService request body: {}",
           req.body);
  if (!authenticateToken(req, res)) {
    LOG_ERROR("RouteController",
              "Authentication failed in deleteHealthcareService");
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
    res.code = 200;
    res.write(msg);
    LOG_INFO("RouteController",
             "deleteHealthcareService success: code={}, id={}", res.code, id);
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController",
              "deleteHealthcareService error: code={}, error={}", res.code,
              e.what());
    res.end();
  }
}

void RouteController::registerUser(const crow::request& req,
                                   crow::response& res) {
  LOG_INFO("RouteController", "registerUser request body: {}", req.body);
  try {
    auto resource = bsoncxx::from_json(req.body);

    if (!resource["email"] || !resource["password"]) {
      res.code = 400;
      std::string message = "Missing required fields: email and password";
      res.write(message);
      LOG_ERROR("RouteController",
                "registerUser validation error: code={}, message={}", res.code,
                message);
      res.end();
      return;
    }

    std::string email = resource["email"].get_utf8().value.to_string();
    std::string password = resource["password"].get_utf8().value.to_string();

    AuthService& authService = AuthService::getInstance();
    std::string token = authService.registerUser(email, password);

    res.code = 201;
    res.write(token);
    LOG_INFO("RouteController", "registerUser success: code={}, user={}",
             res.code, email);
    res.end();
  } catch (const UserAlreadyExistsException& e) {
    LOG_ERROR("RouteController", "User registration failed: {}", e.what());
    res.code = 409;
    res.write(e.what());
    res.end();
  } catch (const AuthException& e) {
    LOG_ERROR("RouteController", "Authentication failed: {}", e.what());
    res.code = 400;
    res.write(e.what());
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController", "registerUser error: code={}, error={}",
              res.code, e.what());
    res.end();
  }
}

void RouteController::loginUser(const crow::request& req, crow::response& res) {
  LOG_INFO("RouteController", "loginUser request body: {}", req.body);
  try {
    auto resource = bsoncxx::from_json(req.body);

    if (!resource["email"] || !resource["password"]) {
      res.code = 400;
      res.write("Missing required fields: email and password");
      LOG_ERROR("RouteController",
                "loginUser validation error: code={}, message={}", res.code,
                "Missing required fields");
      res.end();
      return;
    }

    std::string email = resource["email"].get_utf8().value.to_string();
    std::string password = resource["password"].get_utf8().value.to_string();

    AuthService& authService = AuthService::getInstance();
    std::string token = authService.loginUser(email, password);

    res.code = 200;
    res.write(token);
    LOG_INFO("RouteController", "loginUser success: code={}, user={}", res.code,
             email);
    res.end();
  } catch (const InvalidCredentialsException& e) {
    LOG_ERROR("RouteController", "Login failed: {}", e.what());
    res.code = 401;
    res.write(e.what());
    res.end();
  } catch (const AuthException& e) {
    LOG_ERROR("RouteController", "Authentication failed: {}", e.what());
    res.code = 400;
    res.write(e.what());
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    LOG_ERROR("RouteController", "loginUser error: code={}, error={}", res.code,
              e.what());
    res.end();
  }
}

void RouteController::subscribeToResources(const crow::request& req,
                                           crow::response& res) {
  if (!authenticatePermissionsToPost(req)) {
    res.code = 403;
    res.write("Unauthorized.");
    res.end();
    return;
  }

  try {
    auto resource = bsoncxx::from_json(req.body);
    std::map<std::string, std::string> content;

    for (auto element : resource.view()) {
      content[element.key().to_string()] = element.get_utf8().value.to_string();
    }

    if (content.find("resources") == content.end() ||
        content.find("city") == content.end() ||
        content.find("contact") == content.end()) {
      res.code = 400;
      res.write(
          "Error: Missing required fields (id, resources, city, or contact).");
      res.end();
      return;
    }

    std::string msg = subscriptionManager.addSubscriber(content);

    res.code = 201;
    res.write(msg);
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    res.end();
  }
}

void RouteController::receiveWebhook(const crow::request& req,
                                     crow::response& res) {
  try {
    std::cout << "Webhook received!" << std::endl;
    std::cout << "Body: " << req.body << std::endl;

    res.code = 200;
    res.write("Webhook received successfully.");
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    res.end();
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
      .methods(crow::HTTPMethod::POST)(
          [this](const crow::request& req, crow::response& res) {
            registerUser(req, res);
          });

  CROW_ROUTE(app, "/auth/login")
      .methods(crow::HTTPMethod::POST)(
          [this](const crow::request& req, crow::response& res) {
            loginUser(req, res);
          });

  CROW_ROUTE(app, "/resources/subscribe")
      .methods(crow::HTTPMethod::POST)(
          [this](const crow::request& req, crow::response& res) {
            subscribeToResources(req, res);
          });

  CROW_ROUTE(app, "/webhook")
      .methods(crow::HTTPMethod::POST)(
          [this](const crow::request& req, crow::response& res) {
            receiveWebhook(req, res);
          });
}
