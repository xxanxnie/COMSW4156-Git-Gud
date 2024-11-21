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
Food::Food(DatabaseManager& db) : db(db) {
  cols = std::vector<std::string>({"Name", "City", "Address", "Description",
                                   "ContactInfo", "HoursOfOperation",
                                   "TargetUser", "Quantity", "ExpirationDate"});
  cleanCache();
}
void Food::cleanCache() {
  for (auto name : cols) {
    format[name] = "";
  }
}
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
  if (db.deleteResource("Food", id)) {
    return "SUC";
  }
  throw std::runtime_error("Food Document with the specified _id not found.");
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
