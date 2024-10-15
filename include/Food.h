#ifndef FOOD_H
#define FOOD_H

#include <string>

class Food {
 public:
  Food(const std::string& type, const std::string& quantity);

  std::string getType() const;
  std::string getQuantity() const;

 private:
  std::string type;
  std::string quantity;
};

#endif
