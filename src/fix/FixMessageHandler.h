// An exchange implementation
//
// MIT License
//
// Copyright (c) 2024 Riccardo Canton
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef FIXEXCHANGE_HPP
#define FIXEXCHANGE_HPP

#include "../core/Exchange.hpp"
#include "../core/OrderType.h"
#include "../core/OrderData.h"
#include <quickfix/Application.h>
#include <quickfix/Session.h>
#include <quickfix/MessageCracker.h>
#include <quickfix/fix42/NewOrderSingle.h>
#include <quickfix/fix42/ExecutionReport.h>
#include <quickfix/fix42/OrderCancelRequest.h>
#include <quickfix/fix42/OrderCancelReplaceRequest.h>

/**
 * @class FixMessageHandler
 * @brief FIX application for the exchange that handles incoming FIX messages.
 */
class FixMessageHandler : public FIX::Application, public FIX::MessageCracker {

public:
    FixMessageHandler(Exchange& exchange) : exchange(exchange) {}

protected:
    // Application overloads
    void onCreate(const FIX::SessionID&) override {}
    void onLogon(const FIX::SessionID& sessionID) override;
    void onLogout(const FIX::SessionID& sessionID) override;
    void toAdmin(FIX::Message&, const FIX::SessionID&) override {}
    void toApp(FIX::Message&, const FIX::SessionID&) override
    EXCEPT(FIX::DoNotSend) {}
    void fromAdmin(const FIX::Message&, const FIX::SessionID&) override
    EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon) {}
    void fromApp(const FIX::Message& message, const FIX::SessionID& sessionID ) override
    EXCEPT( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType );
    
    // Handle NewOrderSingle messages
    void onMessage(const FIX42::NewOrderSingle& message, const FIX::SessionID& sessionID) override;

    // Handle OrderCancelReplaceRequest messages
    void onMessage(const FIX42::OrderCancelReplaceRequest& message, const FIX::SessionID& sessionID) override;
    
    // Handle OrderCancelRequest messages
    void onMessage(const FIX42::OrderCancelRequest& message, const FIX::SessionID& sessionID) override;
    
private:
    Exchange& exchange;
};

#endif /* FIXEXCHANGE_HPP */
