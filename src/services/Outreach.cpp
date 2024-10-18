#include "Outreach.h"

// Constructor
Outreach::Outreach(int id, const std::string& targetAudience, const std::string& programName,
                   const std::string& description, const std::string& startDate,
                   const std::string& endDate, const std::string& location,
                   const std::string& contactInfo, DatabaseManager& db)
    : id(id), targetAudience(targetAudience), programName(programName),
      description(description), dates(std::make_pair(startDate, endDate)),
      location(location), contactInfo(contactInfo), dbManager(db) {}

// Getters
int Outreach::getID() const {
    return id;
}

std::string Outreach::getTargetAudience() const {
    return targetAudience;
}

std::string Outreach::getProgramName() const { return programName; }

std::string Outreach::getDescription() const {
    return description;
}

std::string Outreach::getStartDate() const {
    return dates.first;
}

std::string Outreach::getEndDate() const {
    return dates.second;
}

std::string Outreach::getLocation() const {
    return location;
}

std::string Outreach::getContactInfo() const {
    return contactInfo;
}

/**
 * Add the outreach program information to our database
 */
std::string Outreach::addOutreach(const std::string& targetAudience, 
                                   const std::string& programName,
                                   const std::string& description, 
                                   const std::string& startDate,
                                   const std::string& endDate, 
                                   const std::string& location,
                                   const std::string& contactInfo) {
    auto content = createDBContent(targetAudience, programName, description, 
                                    startDate, endDate, location, 
                                    contactInfo);
    try {
        dbManager.insertResource("Outreach", content); // Use dbManager
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return "Error";
    }
    return "Success";
}

/**
 * Create content for the database insertion
 */
std::vector<std::pair<std::string, std::string>> Outreach::createDBContent(
    const std::string& targetAudience, const std::string& programName,
    const std::string& description, const std::string& startDate,
    const std::string& endDate, const std::string& location,
    const std::string& contactInfo) {
    
    std::vector<std::pair<std::string, std::string>> content;
    content.push_back({"_id", std::to_string(id)});
    content.push_back({"targetAudience", targetAudience});
    content.push_back({"programName", programName});
    content.push_back({"description", description});
    content.push_back({"startDate", startDate});
    content.push_back({"endDate", endDate});
    content.push_back({"location", location});
    content.push_back({"contactInfo", contactInfo});
    return content;
}

/**
 * Search all outreach programs in the database
 */
std::string Outreach::searchOutreachAll() {
    std::vector<bsoncxx::document::value> result; // Use document::value
    std::vector<std::pair<std::string, std::string>> keyValues; // Empty key-value pairs

    dbManager.findCollection("Outreach", keyValues, result); // Call with document::value
    std::string ret;

    if (!result.empty()) {
        // Convert document::value to document::view for the print function
        std::vector<bsoncxx::document::view> views;
        views.reserve(result.size()); // Reserve space for efficiency
        for (const auto& docValue : result) {
            views.push_back(docValue.view()); // Convert to view
        }

        ret = printOutreachPrograms(views); // Pass vector of views
        getOutreachID(result[0].view()); // Pass the view of the first document value
    }

    return ret;
}

/**
 * Get the ID of a specific outreach program
 */
std::string Outreach::getOutreachID(const bsoncxx::document::view& outreach) {
    // Ensure that the _id field exists and is of the correct type
    if (auto idElement = outreach["_id"]; idElement && idElement.type() == bsoncxx::type::k_oid) {
        std::string id = idElement.get_oid().value.to_string();
        std::cout << id << std::endl;
        return id;
    } else {
        std::cerr << "Invalid or missing _id field" << std::endl;
        return "";
    }
}

/**
 * Print details of the outreach programs
 */
std::string Outreach::printOutreachPrograms(const std::vector<bsoncxx::document::view>& docs) const {
    std::string output;
    for (const auto& doc : docs) {
        output += bsoncxx::to_json(doc) + "\n"; // Convert BSON to JSON string
    }
    return output;
}
