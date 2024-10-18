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

void RouteController::addOutreach(const crow::request& req, crow::response& res) {
    try {
        auto resource = bsoncxx::from_json(req.body);

        // Check if "_id" exists and is of the correct type
        if (auto idElement = resource.view()["_id"]; idElement && idElement.type() == bsoncxx::type::k_oid) {
            std::string outreachId = idElement.get_oid().value.to_string();

            Outreach outreach(
                std::stoi(outreachId), // Ensure outreachId can be converted to int
                std::string(resource.view()["targetAudience"].get_string().value),
                std::string(resource.view()["programName"].get_string().value),
                std::string(resource.view()["description"].get_string().value),
                std::string(resource.view()["startDate"].get_string().value),
                std::string(resource.view()["endDate"].get_string().value),
                std::string(resource.view()["location"].get_string().value),
                std::string(resource.view()["contactInfo"].get_string().value),
                dbManager // Pass dbManager directly
            );

            // Call the appropriate method to insert outreach
            std::string result = outreach.addOutreach(
                outreach.getTargetAudience(), // Ensure to pass the correct parameters
                outreach.getProgramName(),
                outreach.getDescription(),
                outreach.getStartDate(),
                outreach.getEndDate(),
                outreach.getLocation(),
                outreach.getContactInfo()
            );
            res.code = 201;  // Created
            res.write(result);
            res.end();
        } else {
            res.code = 400; // Bad Request
            res.write("Invalid or missing _id field.");
            res.end();
        }
    } catch (const std::exception& e) {
        res = handleException(e);
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

  CROW_ROUTE(app, "/resources/outreach")
      .methods(crow::HTTPMethod::POST)(
          [this](const crow::request& req, crow::response& res) {
            addOutreach(req, res);
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
