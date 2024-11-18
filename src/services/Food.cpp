// Copyright 2024 COMSW4156-Git-Gud

#include "Food.h"

#include <iostream>

Food::Food(DatabaseManager& db) : db(db) {}

/**
 * @brief Adds a food resource to the database.
 *
 * This method takes a vector of key-value pairs representing the food resource
 * and stores it in the database. The expected key-value pairs for the food
 * resource include:
 * - FoodType: Type of food (e.g., "Fruits")
 * - Provider: The provider of the food (e.g., "LocalFarm")
 * - Location: The location of the provider (e.g., "Brooklyn")
 * - Quantity: The amount of food available (e.g., "100")
 * - ExpirationDate: The date the food expires (e.g., "2024-12-31")
 *
 * @param resource A vector of key-value pairs representing the food resource to
 * be added.
 *
 * @return std::string Returns item ID if the food resource is added
 * successfully, or an error message if the insertion fails.
 *
 * @exception std::exception Throws if an error occurs during the database
 * insertion.
 */
std::string Food::addFood(
    const std::vector<std::pair<std::string, std::string>>& resource) {
  try {
    std::string ID = db.insertResource("Food", resource);
    return ID;
  } catch (const std::exception& e) {
    std::cerr << "Error inserting food resource: " << e.what() << std::endl;
    return "Error inserting food resource: " + std::string(e.what());
  }
}

/**
 * @brief Deletes a food resource from the database.
 *
 * This method takes the ID of the food resource and removes it from the
 * database.
 *
 * @param id The ID of the food resource to be deleted.
 *
 * @return std::string Returns "Success" if the food resource is deleted
 * successfully, or an error message if the deletion fails.
 *
 * @exception std::exception Throws if an error occurs during the database
 * deletion.
 */
std::string Food::deleteFood(const std::string& id) {
  try {
    db.deleteResource("Food", id);
    return "Success";
  } catch (const std::exception& e) {
    std::cerr << "Error deleting food resource: " << e.what() << std::endl;
    return "Error deleting food resource: " + std::string(e.what());
  }
}

/**
 * @brief Retrieves all food resources from the database.
 *
 * This method queries the database for all food resources and returns them
 * as a JSON-formatted string. If no food items are found, it returns an empty
 * JSON array.
 *
 * @return A string containing all food resources in JSON format.
 *
 * @exception std::exception Throws if an error occurs during the database query
 * or data serialization.
 */
std::string Food::getAllFood() {
  std::vector<bsoncxx::document::value> foodItems;

  db.findCollection("Food", {}, foodItems);

  if (foodItems.empty()) {
    return "[]";
  }

  bsoncxx::builder::basic::array arrayBuilder;
  for (const auto& doc : foodItems) {
    arrayBuilder.append(doc.view());
  }

  return bsoncxx::to_json(arrayBuilder.view());
}

/**
 * @brief Updates a food resource in the database.
 *
 * This method takes the ID of the food resource and new key-value pairs
 * to update the resource in the database.
 *
 * @param id The ID of the food resource to be updated.
 * @param resource A vector of key-value pairs with the updated values.
 *
 * @return std::string Returns "Success" if the food resource is updated
 * successfully, or an error message if the update fails.
 *
 * @exception std::exception Throws if an error occurs during the database
 * update.
 */
std::string Food::updateFood(
    const std::string& id,
    const std::vector<std::pair<std::string, std::string>>& resource) {
  try {
    db.updateResource("Food", id, resource);
    return "Success";
  } catch (const std::exception& e) {
    std::cerr << "Error updating food resource: " << e.what() << std::endl;
    return "Error updating food resource: " + std::string(e.what());
  }
}
