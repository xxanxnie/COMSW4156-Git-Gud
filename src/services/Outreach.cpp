#include "Outreach.h"

Outreach::Outreach(const std::string& programName, const std::string& targetGroup)
    : programName(programName), targetGroup(targetGroup) {}

std::string Outreach::getProgramName() const {
    return programName;
}

std::string Outreach::getTargetGroup() const {
    return targetGroup;
}
