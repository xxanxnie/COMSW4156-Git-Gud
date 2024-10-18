// C++ System Header
#include "RouteController.h"
#include "Outreach.h"

#include <bsoncxx/json.hpp>
#include <exception>
#include <iostream>
#include <bsoncxx/json.hpp>
#include "RouteController.h"
#include "Food.h"
#include "Healthcare.h"
#include "Outreach.h"
#include <map>
#include <string>
// Utility function to handle exceptions
crow::response handleException(const std::exception& e) {
  std::cerr << "Error: " << e.what() << std::endl;
  return crow::response{500, "An error has occurred: " + std::string(e.what())};
  std::cerr << "Error: " << e.what() << std::endl;
  return crow::response{500, "An error has occurred: " + std::string(e.what())};
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
  try {
    // Parse the request body into a BSON document
    Shelter s(dbManager, "Shelter");
    std::string response = s.searchShelterAll();
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
  try {
    // Parse the request body into a BSON document
    auto resource = bsoncxx::from_json(req.body);
    Shelter s(dbManager, "Shelter");
    std::vector<std::string> content;
    for (auto element : resource.view()) {
      if (element.key().to_string() != "id") {
        content.push_back(element.get_utf8().value.to_string());
      }
    }
    s.addShelter(content[0], content[1], content[2], atoi(content[3].c_str()),
                 atoi(content[4].c_str()));
    res.code = 201;  // Created
    res.write("Shelter resource added successfully.");
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
  }
}

// Add these new methods to the RouteController class
void RouteController::getCounseling(const crow::request& req, crow::response& res) {
  try {
    Counseling c(dbManager);
    std::string response = c.searchCounselorsAll();
    res.code = 200;
    res.write(response);
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
  }
}

void RouteController::addCounseling(const crow::request& req, crow::response& res) {
  try {
    auto resource = bsoncxx::from_json(req.body);
    Counseling c(dbManager);
    std::string counselorName = resource["counselorName"].get_utf8().value.to_string();
    std::string specialty = resource["specialty"].get_utf8().value.to_string();
    
    std::string result = c.addCounselor(counselorName, specialty);
    
    if (result == "Success") {
      res.code = 201;  // Created
      res.write("Counseling resource added successfully.");
    } else {
      res.code = 400;  // Bad Request
      res.write(result);
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
  }
}

void RouteController::updateCounseling(const crow::request& req, crow::response& res) {
  try {
    auto resource = bsoncxx::from_json(req.body);
    if (!resource["id"]) {
      res.code = 400;
      res.write("Error: Counselor ID is required.");
      res.end();
      return;
    }

    std::string id = resource["id"].get_utf8().value.to_string();
    std::string field = resource["field"].get_utf8().value.to_string();
    std::string value = resource["value"].get_utf8().value.to_string();

    Counseling c(dbManager);
    std::string result = c.updateCounselor(id, field, value);

    if (result == "Update") {
      res.code = 200;
      res.write("Counseling resource updated successfully.");
    } else {
      res.code = 400;
      res.write(result);
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
  }
}

void RouteController::deleteCounseling(const crow::request& req, crow::response& res) {
  try {
    const char* counselorId = req.url_params.get("id");
    if (!counselorId) {
      res.code = 400;
      res.write("Error: Counselor ID is required.");
      res.end();
      return;
    }

    Counseling c(dbManager);
    std::string result = c.deleteCounselor(counselorId);

    if (result == "Delete") {
      res.code = 200;
      res.write("Counseling resource deleted successfully.");
    } else {
      res.code = 400;
      res.write(result);
    }
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
  }
}

void RouteController::addFood(const crow::request& req, crow::response& res) {
    try {
        // Parse the request body into a BSON document
        auto resource = bsoncxx::from_json(req.body);

        // Convert bsoncxx::document::value to vector of pairs
        std::vector<std::pair<std::string, std::string>> keyValues;
        for (auto element : resource.view()) {
            keyValues.emplace_back(element.key().to_string(), element.get_utf8().value.to_string());
        }

        Food food(dbManager);

        food.insertFood(keyValues);

        res.code = 201; // Created
        res.write("Food resource added successfully.");
        res.end();
    } catch (const std::exception& e) {
        res = handleException(e);
    }
}

void RouteController::getAllFood(const crow::request& req, crow::response& res) {
    try {
        // Instantiate the Food class and pass the dbManager
        Food food(dbManager);

        // Fetch all food resources from the database
        std::string response = food.getAllFood();

        // Send the response
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
 * the OutreachService class. It constructs the response based on 
 * the outcome of the operation.
 * 
 * @param req The HTTP request containing the outreach service data in JSON format.
 * @param res The HTTP response that will be sent back to the client.
 * 
 * @throws std::runtime_error if there is not enough data to add the outreach service.
 */
void RouteController::addOutreachService(const crow::request& req, crow::response& res) {
    try {
        auto resource = bsoncxx::from_json(req.body);
        OutreachService os(dbManager, "OutreachService"); // Initialize the OutreachService
        std::vector<std::string> content;
        
        for (auto element : resource.view()) {
            if (element.key().to_string() != "id") { // Exclude ID if present
                content.push_back(element.get_utf8().value.to_string());
            }
        }
        
        if (content.size() < 6) {
            throw std::runtime_error("Not enough data to add OutreachService.");
        }

        os.addOutreachService(content[0], content[1], content[2], content[3],
                               content[4], content[5]);

        res.code = 201; 
        res.write("OutreachService resource added successfully.");
        res.end();
    } catch (const std::exception& e) {
        res = handleException(e); // Custom exception handling
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
void RouteController::getAllOutreachServices(const crow::request& req, crow::response& res) {
    try {
        OutreachService os(dbManager, "OutreachService"); // Initialize the OutreachService
        std::string response = os.getAllOutreachServices(); // Retrieve all outreach services
        res.code = 200;
        res.write(response);
        res.end();
    } catch (const std::exception& e) {
        res = handleException(e); // Custom exception handling
    }
}

void RouteController::addHealthcareService(const crow::request& req,
                                 crow::response& res) {
  try {
    auto resource = bsoncxx::from_json(req.body);
    HealthcareService hs(dbManager, "HealthcareService");
    std::vector<std::string> content;
    for (auto element : resource.view()) {
      if (element.key().to_string() != "id") {
        content.push_back(element.get_utf8().value.to_string());
      }
    }
    hs.addHealthcareService(content[0], content[1], content[2], content[3],
                               content[4], content[5]);

    res.code = 201; 
    res.write("HealthcareService resource added successfully.");
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
  }
}

void RouteController::getAllHealthcareServices(const crow::request& req,
                                 crow::response& res) {
  try {
    // Parse the request body into a BSON document
    HealthcareService hs(dbManager, "HealthcareService");
    std::string response = hs.getAllHealthcareServices();
    res.code = 200;
    res.write(response);
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
  }
}

// Initialize API Routes
void RouteController::initRoutes(crow::SimpleApp& app) {
  CROW_ROUTE(app, "/").methods(crow::HTTPMethod::GET)(
      [this](const crow::request& req, crow::response& res) { index(res); });

  CROW_ROUTE(app, "/resources/addFood")
    .methods(crow::HTTPMethod::POST)(
          [this](const crow::request& req, crow::response& res) {
          addFood(req, res);
  });

  CROW_ROUTE(app, "/resources/getAllFood")
      .methods(crow::HTTPMethod::GET)(
        [this](const crow::request& req, crow::response& res) {
          getAllFood(req, res);
  });
  CROW_ROUTE(app, "/resources/shelter")
      .methods(crow::HTTPMethod::POST)(
          [this](const crow::request& req, crow::response& res) {
            addShelter(req, res);
          });
  CROW_ROUTE(app, "/resources/shelter")
      .methods(crow::HTTPMethod::GET)(
          [this](const crow::request& req, crow::response& res) {
            getShelter(req, res);
          });

  CROW_ROUTE(app, "/resources/counseling")
    .methods(crow::HTTPMethod::GET)(
      [this](const crow::request& req, crow::response& res) {
        getCounseling(req, res);
      });

  CROW_ROUTE(app, "/resources/counseling")
    .methods(crow::HTTPMethod::POST)(
      [this](const crow::request& req, crow::response& res) {
        addCounseling(req, res);
      });

  CROW_ROUTE(app, "/resources/counseling")
    .methods(crow::HTTPMethod::PATCH)(
      [this](const crow::request& req, crow::response& res) {
        updateCounseling(req, res);
      });

  CROW_ROUTE(app, "/resources/counseling")
    .methods(crow::HTTPMethod::DELETE)(
      [this](const crow::request& req, crow::response& res) {
        deleteCounseling(req, res);
        });

  CROW_ROUTE(app, "/resources/outreach/add")
      .methods(crow::HTTPMethod::POST)([this](const crow::request& req, crow::response& res) {
            addOutreachService(req, res);
        });
        
  CROW_ROUTE(app, "/resources/outreach/getAll")
      .methods(crow::HTTPMethod::GET)([this](const crow::request& req, crow::response& res) {
            getAllOutreachServices(req, res);
        });

  CROW_ROUTE(app, "/resources/healthcare/add")
      .methods(crow::HTTPMethod::POST)([this](const crow::request& req, crow::response& res) {
            addHealthcareService(req, res);
        });
        
  CROW_ROUTE(app, "/resources/healthcare/getAll")
      .methods(crow::HTTPMethod::GET)([this](const crow::request& req, crow::response& res) {
            getAllHealthcareServices(req, res);
        });
}
