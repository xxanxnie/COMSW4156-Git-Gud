#ifndef COUNSELING_H
#define COUNSELING_H

#include <string>

class Counseling {
public:
    Counseling(const std::string& counselorName, const std::string& specialty);
    
    std::string getCounselorName() const;
    std::string getSpecialty() const;

private:
    std::string counselorName;
    std::string specialty;
};

#endif 
