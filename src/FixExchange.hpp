#ifndef FIXEXCHANGE_HPP
#define FIXEXCHANGE_HPP

#include "Exchange.hpp"
#include <quickfix/Application.h>
#include <quickfix/Session.h>
#include <quickfix/MessageCracker.h>

/**
 * @class FixExchange
 * @brief FIX application for the exchange that handles incoming FIX messages.
 */
class FixExchange : public FIX::Application, public FIX::MessageCracker {
    Exchange& exchange;

public:
    FixExchange(Exchange& exchange) : exchange(exchange) {}

protected:
    void onCreate(const FIX::SessionID& sessionID) override {}
    void onLogon(const FIX::SessionID& sessionID) override {}
    void onLogout(const FIX::SessionID& sessionID) override {}
    void toAdmin(FIX::Message& message, const FIX::SessionID& sessionID) override {}
    void toApp(FIX::Message& message, const FIX::SessionID& sessionID) override {}
    void fromAdmin(const FIX::Message& message, const FIX::SessionID& sessionID) override {}
    void fromApp(const FIX::Message& message, const FIX::SessionID& sessionID) override {
        crack(message, sessionID);
    }
};

#endif /* FIXEXCHANGE_HPP */
