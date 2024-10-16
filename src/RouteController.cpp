// C++ System Header
#include "RouteController.h"
#include "Outreach.h"

#include <bsoncxx/json.hpp>
#include <exception>
#include <iostream>
#include <map>
#include <string>

// Utility function to handle exceptions
crow::response handleException(const std::exception& e) {
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
}

// Get resources route
void RouteController::getResources(const crow::request& req,
                                   crow::response& res) {
  try {
    const char* resourceType = req.url_params.get("type");
    if (!resourceType) {
      res.code = 400;
      res.write("Error: Resource type is required.");
      res.end();
      return;
    }

    auto resources = dbManager.getResources(resourceType);
    res.code = 200;
    res.write(bsoncxx::to_json(resources));
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
  }
}

// Add resource route
void RouteController::addResource(const crow::request& req,
                                  crow::response& res) {
  try {
    auto resource = bsoncxx::from_json(req.body);
    std::vector<std::pair<std::string, std::string>> keyValues;
    for (auto element : resource.view()) {
      keyValues.emplace_back(element.key().to_string(),
                             element.get_string().value.to_string()); 
    }
    dbManager.insertResource("Resources", keyValues);

    res.code = 201;  
    res.write("Resource added successfully.");
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
  }
}

// Update resource route
void RouteController::updateResource(const crow::request& req,
                                     crow::response& res) {
  try {
    auto resource = bsoncxx::from_json(req.body);
    if (!resource["id"]) {
      res.code = 400;
      res.write("Error: Resource ID is required.");
      res.end();
      return;
    }

    auto id = resource["id"].get_string().value.to_string();
    std::vector<std::pair<std::string, std::string>> updates;
    for (auto element : resource.view()) {
      if (element.key().to_string() != "id") {
        updates.emplace_back(element.key().to_string(),
                             element.get_string().value.to_string()); 
      }
    }
    dbManager.updateResource("Resources", id, updates);

    res.code = 200;
    res.write("Resource updated successfully.");
    res.end();
  } catch (const std::exception& e) {
    res = handleException(e);
  }
}

// Delete resource route
void RouteController::deleteResource(const crow::request& req,
                                     crow::response& res) {
  try {
    const char* resourceId = req.url_params.get("id");
    if (!resourceId) {
      res.code = 400;
      res.write("Error: Resource ID is required.");
      res.end();
      return;
    }

    dbManager.deleteResource("Resources", resourceId);
    res.code = 200;
    res.write("Resource deleted successfully.");
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

// Initialize API Routes
void RouteController::initRoutes(crow::SimpleApp& app) {
  CROW_ROUTE(app, "/").methods(crow::HTTPMethod::GET)(
      [this](const crow::request& req, crow::response& res) { index(res); });

  CROW_ROUTE(app, "/resources")
      .methods(crow::HTTPMethod::GET)(
          [this](const crow::request& req, crow::response& res) {
            getResources(req, res);
          });

  CROW_ROUTE(app, "/resources/add")
      .methods(crow::HTTPMethod::POST)(
          [this](const crow::request& req, crow::response& res) {
            addResource(req, res);
          });

  CROW_ROUTE(app, "/resources/update")
      .methods(crow::HTTPMethod::PATCH)(
          [this](const crow::request& req, crow::response& res) {
            updateResource(req, res);
          });

  CROW_ROUTE(app, "/resources/delete")
      .methods(crow::HTTPMethod::DELETE)(
          [this](const crow::request& req, crow::response& res) {
            deleteResource(req, res);
          });

  CROW_ROUTE(app, "/resources/outreach")
      .methods(crow::HTTPMethod::POST)(
          [this](const crow::request& req, crow::response& res) {
            addOutreach(req, res);
          });
}
