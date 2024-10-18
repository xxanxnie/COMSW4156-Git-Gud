#ifndef OUTREACH_H
#define OUTREACH_H

#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include "DatabaseManager.h"

class Outreach {
public:
    Outreach(int id, const std::string& targetAudience, const std::string& programName,
             const std::string& description, const std::string& startDate,
             const std::string& endDate, const std::string& location,
             const std::string& contactInfo, DatabaseManager& db);

    // Getters
    int getID() const;
    std::string getTargetAudience() const;
    std::string getProgramName() const;
    std::string getDescription() const;
    std::string getStartDate() const;
    std::string getEndDate() const;
    std::string getLocation() const;
    std::string getContactInfo() const;

    // Database operations
    std::string addOutreach(const std::string& targetAudience,
                            const std::string& programName,
                            const std::string& description,
                            const std::string& startDate,
                            const std::string& endDate,
                            const std::string& location,
                            const std::string& contactInfo);

private:
    int id;
    std::string targetAudience;
    std::string programName;
    std::string description;
    std::pair<std::string, std::string> dates; // startDate, endDate
    std::string location;
    std::string contactInfo;
    DatabaseManager& dbManager;

    std::vector<std::pair<std::string, std::string>> createDBContent(
        const std::string& targetAudience, const std::string& programName,
        const std::string& description, const std::string& startDate,
        const std::string& endDate, const std::string& location,
        const std::string& contactInfo);

    std::string printOutreachPrograms(const std::vector<bsoncxx::document::view>& docs) const; 
    std::string getOutreachID(const bsoncxx::document::view& outreach);
    std::string searchOutreachAll();
    //std::string updateOutreach();
    //std::string deleteOutreach();
};

#endif // OUTREACH_H
