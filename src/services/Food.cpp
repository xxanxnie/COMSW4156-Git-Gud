#include "Food.h"

Food::Food(const std::string& type, const std::string& quantity)
    : type(type), quantity(quantity) {}

std::string Food::getType() const {
    return type;
}

std::string Food::getQuantity() const {
    return quantity;
}
