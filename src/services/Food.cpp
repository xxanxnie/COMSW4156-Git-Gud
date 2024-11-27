// Copyright 2024 COMSW4156-Git-Gud

#include "Food.h"

#include <iostream>

/*
Name: Provider
City
Address
Description: FoodType
ContactInfo
Hours of Operation
TargetUser
Quantity
ExpirationDate
*/

/**
 * @brief Constructs a Food object.
 * @param db Reference to the DatabaseManager object.
 */
Food::Food(DatabaseManager& db, const std::string& collection_name)
    : db(db), collection_name(collection_name) {
  cols = std::vector<std::string>({"Name", "City", "Address", "Description",
                                   "ContactInfo", "HoursOfOperation",
                                   "TargetUser", "Quantity", "ExpirationDate"});
  cleanCache();
}
/**
 * @brief Resets the cache format to an empty state.
 */
void Food::cleanCache() {
  for (auto name : cols) {
    format[name] = "";
  }
}
/**
 * @brief Validates the input JSON format and extracts the ID if present.
 *
 * This method ensures all required fields are provided and valid in the
 * request body. It also checks that the quantity is a positive integer.
 *
 * @param content A JSON string containing the food resource data.
 *
 * @return The extracted ID as a string, if present in the input.
 *
 * @throws std::invalid_argument If the input is empty, missing required fields,
 * or contains invalid data.
 */
std::string Food::checkInputFormat(std::string content) {
  if (content.empty()) {
    throw std::invalid_argument("Invalid input: Request body cannot be empty.");
  }
  auto resource = bsoncxx::from_json(content);
  std::string id;
  for (auto element : resource.view()) {
    if (format.find(element.key().to_string()) != format.end()) {
      format[element.key().to_string()] = element.get_utf8().value.to_string();
    } else {
      if (element.key().to_string() == "id") {
        id = element.get_utf8().value.to_string();
        continue;
      }
      throw std::invalid_argument("The request with unrelative argument.");
    }
  }
  int capacity = atoi(format["Quantity"].c_str());

  if (capacity <= 0) {
    throw std::invalid_argument("The request with invalid argument.");
  }
  for (auto property : format) {
    if (property.second == "") {
      throw std::invalid_argument("The request missing some properties.");
    }
  }
  return id;
}
/**
 * @brief Creates a vector of key-value pairs representing the food resource.
 *
 * @return A vector containing all key-value pairs for the food resource.
 */
std::vector<std::pair<std::string, std::string>> Food::createDBContent() {
  std::vector<std::pair<std::string, std::string>> content;
  for (auto property : format) {
    content.push_back(property);
  }
  return content;
}
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
std::string Food::addFood(std::string request_body) {
  try {
    cleanCache();
    checkInputFormat(request_body);
    auto content_new = createDBContent();
    std::string ID = db.insertResource("Food", content_new);
    return ID;
  } catch (const std::exception& e) {
    std::cerr << "Error inserting food resource: " << e.what() << std::endl;
    return "Error inserting food resource: " + std::string(e.what());
  }
}
/**
 * @brief Deletes a food resource from the database.
 *
 * This method removes a food resource from the database by its ID.
 *
 * @param id The ID of the food resource to be deleted.
 *
 * @return "Success" if the resource is deleted, or an error message if deletion
 * fails.
 *
 * @throws std::runtime_error If the specified document is not found in the
 * database.
 */
std::string Food::deleteFood(const std::string& id) {
  if (db.deleteResource("Food", id)) {
    return "SUC";
  }
  throw std::runtime_error("Food Document with the specified _id not found.");
}

/**
 * @brief Retrieves all food resources from the database.
 *
 * This method queries the database for all food resources and returns them as
 * a JSON string. If no food items are found, it returns an empty JSON array.
 *
 * @param start The starting index for pagination.
 *
 * @return A JSON string containing all food resources in the database.
 *
 * @throws std::exception If an error occurs during the database query or
 * serialization.
 */
std::string Food::getAllFood(int start) {
  std::vector<bsoncxx::document::value> foodItems;

  db.findCollection(start, "Food", {}, foodItems);

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
 * This method updates a food resource in the database based on its ID and the
 * provided JSON request body.
 *
 * @param request_body A JSON string containing the updated food resource
 * details.
 *
 * @return "Success" if the resource is updated successfully, or an error
 * message if the update fails.
 *
 * @throws std::exception If an error occurs during database update.
 */
std::string Food::updateFood(std::string request_body) {
  try {
    cleanCache();
    std::string id = checkInputFormat(request_body);
    auto content_new = createDBContent();
    db.updateResource("Food", id, content_new);
    return "Success";
  } catch (const std::exception& e) {
    std::cerr << "Error updating food resource: " << e.what() << std::endl;
    return "Error updating food resource: " + std::string(e.what());
  }
}
