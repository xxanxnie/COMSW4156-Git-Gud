#ifndef SHELTER_H
#define SHELTER_H

#include <string>

class Shelter {
public:
    Shelter(const std::string& name, const std::string& location);
    
    std::string getName() const;
    std::string getLocation() const;

private:
    std::string name;
    std::string location;
};

#endif
