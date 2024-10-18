#include "Healthcare.h"
#include "DatabaseManager.h"
#include <iostream>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/oid.hpp>

std::string HealthcareService::addHealthcareService(const std::string& provider, const std::string& serviceType, 
                                                    const std::string& location, const std::string& operatingHours, 
                                                    const std::string& eligibilityCriteria, const std::string& contactInfo) {
    try {
        auto content = createDBContent(provider, serviceType, location, operatingHours, eligibilityCriteria, contactInfo);
        dbManager.insertResource(collection_name, content);
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return "Error: " + std::string(e.what());
    }
    return "Success";
}

std::vector<std::pair<std::string, std::string>> HealthcareService::createDBContent(const std::string& provider, 
                                                                                    const std::string& serviceType, 
                                                                                    const std::string& location, 
                                                                                    const std::string& operatingHours, 
                                                                                    const std::string& eligibilityCriteria, 
                                                                                    const std::string& contactInfo) {
    std::vector<std::pair<std::string, std::string>> content;
    content.push_back({"provider", provider});
    content.push_back({"serviceType", serviceType});
    content.push_back({"location", location});
    content.push_back({"operatingHours", operatingHours});
    content.push_back({"eligibilityCriteria", eligibilityCriteria});
    content.push_back({"contactInfo", contactInfo});
    return content;
}

std::string HealthcareService::getAllHealthcareServices() {
    std::vector<bsoncxx::document::value> result;
    dbManager.findCollection(collection_name, {}, result);  
    if (result.empty()) {
        return "[]";
    }
    return printHealthcareServices(result);
}

std::string HealthcareService::printHealthcareServices(
    std::vector<bsoncxx::document::value> &services) const {
  std::string ret;
  for (auto hs : services) {
    for (auto element : hs.view()) {
      if (element.type() != bsoncxx::type::k_oid) {
        ret += element.get_string().value.to_string()+ "\n";
      }
      ret += "\n";
    }
  }
  return ret;
}

