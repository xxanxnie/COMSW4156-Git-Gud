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
  if (!authenticatePermissionsToGetAll(req)) {
    res.code = 403;
    res.write("Unauthorized.");
    res.end();
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
  if (!authenticatePermissionsToPost(req)) {
    res.code = 403;
    res.write("Unauthorized.");
    res.end();
    return;
  }

  try {
    auto resource = bsoncxx::from_json(req.body);
    std::vector<std::string> content;
    for (auto element : resource.view()) {
      if (element.key().to_string() != "id") {
        content.push_back(element.get_utf8().value.to_string());
      }
    }
    shelterManager.addShelter(content[0], content[1], content[2],
                              atoi(content[3].c_str()),
                              atoi(content[4].c_str()));
    res.code = 201;
    res.write("Shelter resource added successfully.");
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
  }
}

void RouteController::deleteShelter(const crow::request& req,
                                    crow::response& res) {
  if (!authenticatePermissionsToPost(req)) {
    res.code = 403;
    res.write("Unauthorized.");
    res.end();
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
    res.code = 201;
    res.write("Shelter resource deleted successfully.");
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
    res.end();
  }
}
// Add these new methods to the RouteController class
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
  if (!authenticatePermissionsToGetAll(req)) {
    res.code = 403;
    res.write("Unauthorized.");
    res.end();
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
  if (!authenticatePermissionsToPost(req)) {
    res.code = 403;
    res.write("Unauthorized.");
    res.end();
    return;
  }

  try {
    auto resource = bsoncxx::from_json(req.body);
    std::string counselorName =
        resource["counselorName"].get_utf8().value.to_string();
    std::string specialty = resource["specialty"].get_utf8().value.to_string();

    std::string result =
        counselingManager.addCounselor(counselorName, specialty);

    if (result == "Success") {
      res.code = 201;
      res.write("Counseling resource added successfully.");
    } else {
      res.code = 400;
      res.write(result);
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
  }
}

// void RouteController::updateCounseling(const crow::request& req,
// crow::response& res) {
//   try {
//     auto resource = bsoncxx::from_json(req.body);
//     if (!resource["id"]) {
//       res.code = 400;
//       res.write("Error: Counselor ID is required.");
//       res.end();
//       return;
//     }

//     std::string id = resource["id"].get_utf8().value.to_string();
//     std::string field = resource["field"].get_utf8().value.to_string();
//     std::string value = resource["value"].get_utf8().value.to_string();

//     Counseling c(dbManager);
//     std::string result = c.updateCounselor(id, field, value);

//     if (result == "Update") {
//       res.code = 200;
//       res.write("Counseling resource updated successfully.");
//     } else {
//       res.code = 400;
//       res.write(result);
//     }
//     res.end();
//   } catch (const std::exception& e) {
//     res = handleException(e);
//   }
// }

// void RouteController::deleteCounseling(const crow::request& req,
// crow::response& res) {
//   try {
//     const char* counselorId = req.url_params.get("id");
//     if (!counselorId) {
//       res.code = 400;
//       res.write("Error: Counselor ID is required.");
//       res.end();
//       return;
//     }

//     Counseling c(dbManager);
//     std::string result = c.deleteCounselor(counselorId);

//     if (result == "Delete") {
//       res.code = 200;
//       res.write("Counseling resource deleted successfully.");
//     } else {
//       res.code = 400;
//       res.write(result);
//     }
//     res.end();
//   } catch (const std::exception& e) {
//     res = handleException(e);
//   }
// }

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
  if (!authenticatePermissionsToPost(req)) {
    res.code = 403;
    res.write("Unauthorized.");
    res.end();
    return;
  }

  try {
    auto resource = bsoncxx::from_json(req.body);

    std::vector<std::pair<std::string, std::string>> keyValues;
    for (auto element : resource.view()) {
      keyValues.emplace_back(element.key().to_string(),
                             element.get_utf8().value.to_string());
    }

    std::string result = foodManager.addFood(keyValues);

    if (result == "Success") {
      res.code = 201;
      res.write("Food resource added successfully.");
    } else {
      res.code = 400;
      res.write(result);
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
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
  if (!authenticatePermissionsToGetAll(req)) {
    res.code = 403;
    res.write("Unauthorized.");
    res.end();
    return;
  }

  try {
    std::string response = foodManager.getAllFood();

    res.code = 200;
    res.write(response);
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
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
  if (!authenticatePermissionsToPost(req)) {
    res.code = 403;
    res.write("Unauthorized.");
    res.end();
    return;
  }

  try {
    auto resource = bsoncxx::from_json(req.body);
    // Initialize the OutreachService
    std::vector<std::string> content;

    for (auto element : resource.view()) {
      if (element.key().to_string() != "id") {  // Exclude ID if present
        content.push_back(element.get_utf8().value.to_string());
      }
    }

    if (content.size() < 6) {
      throw std::runtime_error("Not enough data to add OutreachService.");
    }

    outreachManager.addOutreachService(content[0], content[1], content[2],
                                       content[3], content[4], content[5]);

    res.code = 201;
    res.write("OutreachService resource added successfully.");
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
  if (!authenticatePermissionsToGetAll(req)) {
    res.code = 403;
    res.write("Unauthorized.");
    res.end();
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
  if (!authenticatePermissionsToPost(req)) {
    res.code = 403;
    res.write("Unauthorized.");
    res.end();
    return;
  }

  try {
    auto resource = bsoncxx::from_json(req.body);
    std::vector<std::string> content;
    for (auto element : resource.view()) {
      if (element.key().to_string() != "id") {
        content.push_back(element.get_utf8().value.to_string());
      }
    }
    healthcareManager.addHealthcareService(content[0], content[1], content[2],
                                           content[3], content[4], content[5]);

    res.code = 201;
    res.write("HealthcareService resource added successfully.");
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
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
  if (!authenticatePermissionsToGetAll(req)) {
    res.code = 403;
    res.write("Unauthorized.");
    res.end();
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
  CROW_ROUTE(app, "/resources/shelter/add")
      .methods(crow::HTTPMethod::POST)(
          [this](const crow::request& req, crow::response& res) {
            addShelter(req, res);
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

  // CROW_ROUTE(app, "/resources/counseling")
  //   .methods(crow::HTTPMethod::PATCH)(
  //     [this](const crow::request& req, crow::response& res) {
  //       updateCounseling(req, res);
  //     });

  // CROW_ROUTE(app, "/resources/counseling")
  //   .methods(crow::HTTPMethod::DELETE)(
  //     [this](const crow::request& req, crow::response& res) {
  //       deleteCounseling(req, res);
  //       });

  CROW_ROUTE(app, "/resources/outreach/add")
      .methods(crow::HTTPMethod::POST)(
          [this](const crow::request& req, crow::response& res) {
            addOutreachService(req, res);
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
}
