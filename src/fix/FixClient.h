#ifndef FIXCLIENT_HPP
#define FIXCLIENT_HPP

#include <quickfix/Application.h>
#include <quickfix/Session.h>
#include <quickfix/MessageCracker.h>
#include <quickfix/fix42/NewOrderSingle.h>
#include <quickfix/fix42/ExecutionReport.h>
#include <quickfix/fix42/OrderCancelRequest.h>
#include <quickfix/fix42/OrderCancelReplaceRequest.h>
#include <quickfix/SocketInitiator.h>
#include <quickfix/FileStore.h>
#include <quickfix/Log.h>

/**
 * @class FixClient
 * @brief A client that connects to the exchange and sends FIX messages.
 */
class FixClient : public FIX::Application, public FIX::MessageCracker {
public:
    FixClient(const std::string& configFile);
    void start();
    void stop();

    // Sending FIX messages
    void sendNewOrderSingle(const std::string& symbol, char side, double price, int quantity, char orderType);
    void sendOrderCancelRequest(const std::string& origClOrdID, const std::string& symbol);
    void sendOrderCancelReplaceRequest(const std::string& origClOrdID, const std::string& symbol, double newPrice, int newQuantity);

protected:
    // Application overrides
    void onCreate(const FIX::SessionID&) override {}
    void onLogon(const FIX::SessionID& sessionID) override;
    void onLogout(const FIX::SessionID& sessionID) override;
    void toAdmin(FIX::Message&, const FIX::SessionID&) override {}
    void toApp(FIX::Message&, const FIX::SessionID&) override EXCEPT(FIX::DoNotSend) {}
    void fromAdmin(const FIX::Message&, const FIX::SessionID&) override EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon) {}
    void fromApp(const FIX::Message& message, const FIX::SessionID& sessionID) override EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType);

    // Handle incoming ExecutionReports
    void onMessage(const FIX42::ExecutionReport& message, const FIX::SessionID& sessionID) override;

private:
    FIX::SessionID sessionID;
    FIX::SocketInitiator* initiator;
    FIX::SessionSettings* settings;
    FIX::FileStoreFactory* storeFactory;
    FIX::ScreenLogFactory* logFactory;
    std::string configFile;
};

#endif /* FIXCLIENT_HPP */

