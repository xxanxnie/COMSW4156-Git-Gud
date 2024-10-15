#ifndef HEALTHCARE_H
#define HEALTHCARE_H

#include <string>

class Healthcare {
 public:
  Healthcare(const std::string& provider, const std::string& serviceType);

  std::string getProvider() const;
  std::string getServiceType() const;

 private:
  std::string provider;
  std::string serviceType;
};

#endif
