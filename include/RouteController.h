#ifndef ROUTECONTROLLER_H
#define ROUTECONTROLLER_H

#include <exception>
#include <iostream>
#include <map>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <string>

#include "../external_libraries/Crow/include/crow.h"
#include "Counseling.h"
#include "DatabaseManager.h"
#include "Food.h"
#include "Healthcare.h"
#include "Outreach.h"
#include "Shelter.h"

class RouteController {
 private:
  DatabaseManager& dbManager;
  Shelter& shelterManager;
  Counseling& counselingManager;
  Healthcare& healthcareManager;
  Outreach& outreachManager;
  Food& foodManager;

 public:
  // Constructor
  RouteController(DatabaseManager& dbManager, Shelter& shelterManager,
                  Counseling& counselingManager, Healthcare& healthcareManager,
                  Outreach& outreachManager, Food& foodManager)
      : dbManager(dbManager),
        shelterManager(shelterManager),
        counselingManager(counselingManager),
        healthcareManager(healthcareManager),
        outreachManager(outreachManager),
        foodManager(foodManager) {}

  // Initialize routes
  void initRoutes(crow::SimpleApp& app);

  // Route handlers
  void index(crow::response& res);

  // Shelter-related handlers
  void addShelter(const crow::request& req, crow::response& res);
  void getShelter(const crow::request& req, crow::response& res);

  // Counseling-related handlers
  void getCounseling(const crow::request& req, crow::response& res);
  void addCounseling(const crow::request& req, crow::response& res);
  // void updateCounseling(const crow::request& req, crow::response& res);
  // void deleteCounseling(const crow::request& req, crow::response& res);

  // Outreach-related handlers
  void addOutreachService(const crow::request& req, crow::response& res);
  void getAllOutreachServices(const crow::request& req, crow::response& res);

  // Food-related handlers
  void addFood(const crow::request& req, crow::response& res);
  void getAllFood(const crow::request& req, crow::response& res);

  // Healthcare-related handlers
  void addHealthcareService(const crow::request& req, crow::response& res);
  void getAllHealthcareServices(const crow::request& req, crow::response& res);
};

#endif
