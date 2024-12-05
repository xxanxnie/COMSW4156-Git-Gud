// Copyright 2024 COMSW4156-Git-Gud
#include "SubscriptionManager.h"

#include <curl/curl.h>
#include <curl/curlver.h>
#include <curl/easy.h>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include "Logger.h"
#include "Poco/Net/AcceptCertificateHandler.h"
#include "Poco/Net/InvalidCertificateHandler.h"
#include "Poco/Net/MailMessage.h"
#include "Poco/Net/MailRecipient.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/PrivateKeyPassphraseHandler.h"
#include "Poco/Net/SMTPClientSession.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/Net/SecureSMTPClientSession.h"
#include "Poco/Net/SecureStreamSocket.h"

using Poco::SharedPtr;
using Poco::Net::AcceptCertificateHandler;
using Poco::Net::Context;
using Poco::Net::InvalidCertificateHandler;
using Poco::Net::MailMessage;
using Poco::Net::MailRecipient;
using Poco::Net::SecureSMTPClientSession;
using Poco::Net::SecureStreamSocket;
using Poco::Net::SMTPClientSession;
using Poco::Net::SocketAddress;
using Poco::Net::SSLManager;

SubscriptionManager::SubscriptionManager(DatabaseManager& dbManager)
    : dbManager(dbManager) {}

/**
 * @brief Adds a subscriber to the database.
 *
 * @param subscriberDetails A map containing subscriber details such as email,
 *        city, and resources.
 * @return std::string A message indicating the success of the operation.
 *
 * @throws std::exception If there is an error during the database operation.
 */
std::string SubscriptionManager::addSubscriber(
    const std::map<std::string, std::string>& subscriberDetails) {
  std::vector<std::pair<std::string, std::string>> keyValues(
      subscriberDetails.begin(), subscriberDetails.end());
  std::string id = dbManager.insertResource("Subscribers", keyValues);
  return id;
}

/**
 * @brief Deletes a subscriber from the database.
 *
 * @param id The unique identifier of the subscriber to delete.
 * @return std::string A message indicating success or failure of the operation.
 *
 * @throws std::exception If there is an error during the database operation.
 */
std::string SubscriptionManager::deleteSubscriber(const std::string& id) {
  if (dbManager.deleteResource("Subscribers", id, "")) {
    return "Subscriber deleted successfully.";
  } else {
    return "Error: Subscriber not found.";
  }
}

/**
 * @brief Retrieves subscribers based on the specified resource and city.
 *
 * @param resource The resource type the subscribers are interested in.
 * @param city The city associated with the subscribers.
 * @return std::map<std::string, std::string> A map of subscriber IDs and their
 *         contact information.
 *
 * @throws std::exception If there is an error during the database query.
 */
std::map<std::string, std::string> SubscriptionManager::getSubscribers(
    const std::string& resource, const std::string& city) {
  std::vector<bsoncxx::document::value> docs;
  std::map<std::string, std::string> subscribers;
  dbManager.findCollection(0, "Subscribers",
                           {{"Resource", resource}, {"City", city}}, docs);

  for (const auto& doc : docs) {
    auto view = doc.view();
    std::string id = view["_id"].get_oid().value.to_string();
    std::string contact = view["Contact"].get_utf8().value.to_string();

    subscribers[id] = contact;
  }

  return subscribers;
}

/**
 * @brief Notifies subscribers about an update to a resource in a city.
 *
 * Sends notifications to subscribers via email or webhook, depending on the
 * format of their contact information.
 *
 * @param resource The resource type that has an update.
 * @param city The city associated with the update.
 *
 * @throws std::exception If there is an error during notification dispatch.
 */
void SubscriptionManager::notifySubscribers(const std::string& resource,
                                            const std::string& city) {
  LOG_INFO("SubscriptionManager",
           "Sending notifications for resource {}, city {}", resource, city);
  std::map<std::string, std::string> subscribers =
      getSubscribers(resource, city);

  for (const auto& [id, contact] : subscribers) {
    if (contact.find('@') != std::string::npos) {
      sendEmail(
          contact, "Notification",
          "A new update for " + resource + " in " + city + " is available.");
    } else {
      sendWebhook(contact, "{\"message\": \"A new update for " + resource +
                               " in " + city + " is available.\"}");
    }
  }
}

/**
 * @brief Sends an email to a specified recipient.
 *
 * Uses a secure SMTP connection to send an email containing a subject and
 * content.
 *
 * @param to The recipient's email address.
 * @param subject The subject of the email.
 * @param content The content/body of the email.
 *
 * @throws Poco::Net::SMTPException If there is an SMTP-specific error.
 * @throws Poco::Net::NetException If there is a general network error.
 */
void SubscriptionManager::sendEmail(const std::string& to,
                                    const std::string& subject,
                                    const std::string& content) {
  LOG_INFO("SubscriptionManager", "About to send email to: {}", to);
  std::string host = "smtp.gmail.com";
  int port = 465;

  std::string sUserName = "gitgud.notifications@gmail.com";
  std::string sPassword = "qnnlbrtqbjgodnjg";
  std::string from = "gitgud.notifications@gmail.com";
  MailMessage message;

  try {
    SharedPtr<InvalidCertificateHandler> pCert =
        new AcceptCertificateHandler(false);
    Context::Ptr pContext = new Poco::Net::Context(
        Context::CLIENT_USE, "", "", "", Context::VERIFY_NONE, 9, false,
        "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
    SSLManager::instance().initializeClient(0, pCert, pContext);

    SecureStreamSocket pSSLSocket(pContext);
    pSSLSocket.connect(SocketAddress(host, port));
    SecureSMTPClientSession secure(pSSLSocket);

    secure.login();
    secure.startTLS(pContext);
    secure.login(SMTPClientSession::AUTH_LOGIN, sUserName, sPassword);

    message.setSender(from);
    message.addRecipient(MailRecipient(MailRecipient::PRIMARY_RECIPIENT, to));
    message.setSubject(subject);
    message.setContentType("text/plain; charset=UTF-8");
    message.setContent(content, MailMessage::ENCODING_8BIT);

    secure.sendMessage(message);
    secure.close();

    LOG_INFO("SubscriptionManager", "Email sent successfully to: {}", to);
  } catch (Poco::Net::SMTPException& e) {
    LOG_ERROR("SubscriptionManager", "SMTPException: {}", e.displayText());
  } catch (Poco::Net::NetException& e) {
    LOG_ERROR("SubscriptionManager", "NetException: {}", e.displayText());
  }
}

/**
 * @brief Sends a webhook notification to a specified URL.
 *
 * Makes an HTTP POST request to send a payload to a given URL.
 *
 * @param url The target URL for the webhook.
 * @param payload The payload to be sent as part of the HTTP POST request.
 *
 * @throws std::exception If there is an error during the HTTP request.
 */
void SubscriptionManager::sendWebhook(const std::string& url,
                                      const std::string& payload) {
  LOG_INFO("SubscriptionManager", "Sending webhook to URL: {}", url);
  CURL* curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      LOG_ERROR("SubscriptionManager", "curl_easy_perform() failed: {}",
                curl_easy_strerror(res));
    } else {
      LOG_INFO("SubscriptionManager", "Webhook sent successfully to: {}", url);
    }

    curl_easy_cleanup(curl);
  } else {
    LOG_ERROR("SubscriptionManager", "Failed to initialize CURL.");
  }
}
