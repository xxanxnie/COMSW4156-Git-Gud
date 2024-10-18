// Copyright 2024 Annie, Xu

#ifndef OUTREACH_H
#define OUTREACH_H

#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include "DatabaseManager.h"

class OutreachService {
public:
    OutreachService(DatabaseManager& dbManager, const std::string& collection_name)
        : dbManager(dbManager), collection_name(collection_name) {}

    std::string collection_name;

    std::string addOutreachService(const std::string& targetAudience,
                            const std::string& programName,
                            const std::string& description,
                            const std::string& programDate,
                            const std::string& location,
                            const std::string& contactInfo);

    std::vector<std::pair<std::string, std::string>> createDBContent(
        const std::string& targetAudience, const std::string& programName,
        const std::string& description, const std::string& programDate,
        const std::string& location, const std::string& contactInfo);

    std::string getAllOutreachServices();
    std::string printOutreachServices(const std::vector<bsoncxx::document::value>& services) const; 

private:
    DatabaseManager& dbManager;
};

#endif // OUTREACH_H