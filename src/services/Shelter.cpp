#include "Shelter.h"

Shelter::Shelter(const std::string& name, const std::string& location)
    : name(name), location(location) {}

std::string Shelter::getName() const {
    return name;
}

std::string Shelter::getLocation() const {
    return location;
}
