// Copyright 2024 COMSW4156-Git-Gud
#include "SubscriptionManager.h"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <curl/curl.h>

#include "Poco/Net/MailMessage.h"
#include "Poco/Net/MailRecipient.h"
#include "Poco/Net/SMTPClientSession.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/SecureSMTPClientSession.h"
#include "Poco/Net/InvalidCertificateHandler.h"
#include "Poco/Net/AcceptCertificateHandler.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/Net/SecureStreamSocket.h"
#include "Poco/Net/MailRecipient.h"

using Poco::Net::InvalidCertificateHandler;
using Poco::Net::AcceptCertificateHandler;
using Poco::Net::Context;
using Poco::Net::SSLManager;
using Poco::Net::SecureStreamSocket;
using Poco::Net::SocketAddress;
using Poco::Net::SecureSMTPClientSession;
using Poco::Net::SMTPClientSession;
using Poco::SharedPtr;
using Poco::Net::MailMessage;
using Poco::Net::MailRecipient;

SubscriptionManager::SubscriptionManager(DatabaseManager& dbManager)
    : dbManager(dbManager) {}

std::string SubscriptionManager::addSubscriber(const std::map<std::string, std::string>& subscriberDetails) {
    std::vector<std::pair<std::string, std::string>> keyValues(subscriberDetails.begin(), subscriberDetails.end());
    dbManager.insertResource("Subscribers", keyValues);
    return "Subscriber added successfully.";
}

std::string SubscriptionManager::deleteSubscriber(const std::string& id) {
    if (dbManager.deleteResource("Subscribers", id)) {
        return "Subscriber deleted successfully.";
    } else {
        return "Error: Subscriber not found.";
    }
}


std::map<std::string, std::string> SubscriptionManager::getSubscribers(const std::string& resource, const std::string& city) {
    std::vector<bsoncxx::document::value> docs;
    std::map<std::string, std::string> subscribers;
    dbManager.findCollection("Subscribers", {{"resources", resource}, {"city", city}}, docs);

    for (const auto& doc : docs) {
        auto view = doc.view();
        std::string id = view["_id"].get_oid().value.to_string();
        std::string contact = view["contact"].get_utf8().value.to_string();

        subscribers[id] = contact;
    }

    return subscribers;
}

void SubscriptionManager::notifySubscribers(const std::string& resource, const std::string& city) {
    std::map<std::string, std::string> subscribers = getSubscribers(resource, city);

    for (const auto& [id, contact] : subscribers) {
        if (contact.find('@') != std::string::npos) {
            // contact is an email
            sendEmail(contact, "Notification", "A new update for " + resource + " in " + city + " is available.");
            // contact is a webhook URL
            sendWebhook(contact, "{\"message\": \"A new update for " + resource + " in " + city + " is available.\"}");
        }
    }
}


void SubscriptionManager::sendEmail(const std::string& to, const std::string& subject, const std::string& content) {
    std::cout << "About to send email.." << std::endl;
    std::string host = "smtp.gmail.com";
    int port = 465;

    std::string sUserName = "gitgud.notifications@gmail.com";
    std::string sPassword = "qnnlbrtqbjgodnjg";
    std::string from = "gitgud.notifications@gmail.com";
    MailMessage message;

    try {
        SharedPtr<InvalidCertificateHandler> pCert = new AcceptCertificateHandler(false);
        Context::Ptr pContext = new Poco::Net::Context(Context::CLIENT_USE, "", "", "", Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
        SSLManager::instance().initializeClient(0, pCert, pContext);

        SecureStreamSocket pSSLSocket(pContext);
        pSSLSocket.connect(SocketAddress(host, port));
        SecureSMTPClientSession secure(pSSLSocket);

        secure.login();
        bool tlsStarted = secure.startTLS(pContext);
        secure.login(SMTPClientSession::AUTH_LOGIN, sUserName, sPassword);

        message.setSender(from);
        message.addRecipient(MailRecipient(MailRecipient::PRIMARY_RECIPIENT, to));
        message.setSubject(subject);
        message.setContentType("text/plain; charset=UTF-8");
        message.setContent(content, MailMessage::ENCODING_8BIT);

        secure.sendMessage(message);
        secure.close();

    } catch (Poco::Net::SMTPException &e) {
       std::cout << e.code() << '\n';
       std::cout << e.message() << '\n';
       std::cout << e.what() << '\n';
       std::cout << e.displayText().c_str() << '\n';
    }
    catch (Poco::Net::NetException &e) {
       std::cout << e.code() << '\n';
       std::cout << e.message() << '\n';
       std::cout << e.what() << '\n';
       std::cout << e.displayText().c_str() << '\n';
    }

    std::cout << "Email sent!";
}

void SubscriptionManager::sendWebhook(const std::string& url, const std::string& payload) {
    // CURL *curl = curl_easy_init();
    // if(curl) {
    //     curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    //     curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

    //     CURLcode res = curl_easy_perform(curl);

    //     if(res != CURLE_OK) {
    //         fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    //     }

    //     curl_easy_cleanup(curl);
    // }
    std::cout << "req sent!";
}
