// Copyright 2024 COMSW4156-Git-Gud
#pragma once

#include <map>
#include <string>
#include <vector>

#include "DatabaseManager.h"

class SubscriptionManager {
 public:
  SubscriptionManager(DatabaseManager& dbManager);
  virtual std::string addSubscriber(
      const std::map<std::string, std::string>& subscriberDetails);
  virtual std::string deleteSubscriber(const std::string& id);
  virtual std::map<std::string, std::string> getSubscribers(
      const std::string& resource, const std::string& city);
  virtual void notifySubscribers(const std::string& resource,
                                 const std::string& city);

 private:
  DatabaseManager& dbManager;

  void sendEmail(const std::string& to, const std::string& subject,
                 const std::string& content);
  void sendWebhook(const std::string& url, const std::string& payload);
};
