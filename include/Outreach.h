#ifndef OUTREACH_H
#define OUTREACH_H

#include <string>

class Outreach {
public:
    Outreach(const std::string& programName, const std::string& targetGroup);
    
    std::string getProgramName() const;
    std::string getTargetGroup() const;

private:
    std::string programName;
    std::string targetGroup;
};

#endif 
