#include "Counseling.h"

Counseling::Counseling(const std::string& counselorName, const std::string& specialty)
    : counselorName(counselorName), specialty(specialty) {}

std::string Counseling::getCounselorName() const {
    return counselorName;
}

std::string Counseling::getSpecialty() const {
    return specialty;
}
