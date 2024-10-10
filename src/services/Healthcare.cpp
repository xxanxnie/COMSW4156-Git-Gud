#include "Healthcare.h"

Healthcare::Healthcare(const std::string& provider, const std::string& serviceType)
    : provider(provider), serviceType(serviceType) {}

std::string Healthcare::getProvider() const {
    return provider;
}

std::string Healthcare::getServiceType() const {
    return serviceType;
}
