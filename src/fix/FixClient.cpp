#include "FixClient.h"
#include <iostream>

FixClient::FixClient(const std::string& configFile) : configFile(configFile) {
    try {
        settings = new FIX::SessionSettings(configFile);
        storeFactory = new FIX::FileStoreFactory(*settings);
        logFactory = new FIX::ScreenLogFactory(*settings);
        initiator = new FIX::SocketInitiator(*this, *storeFactory, *settings, *logFactory);
    } catch (const std::exception& e) {
        std::cerr << "Error initializing FixClient: " << e.what() << std::endl;
        throw;
    }
}

void FixClient::start() {
    try {
        initiator->start();
        std::cout << "FIX Client started" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error starting FixClient: " << e.what() << std::endl;
    }
}

void FixClient::stop() {
    try {
        initiator->stop();
        std::cout << "FIX Client stopped" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error stopping FixClient: " << e.what() << std::endl;
    }
}

void FixClient::sendNewOrderSingle(const std::string& symbol, char side, double price, int quantity, char orderType) {
    FIX::ClOrdID clOrdID(std::to_string(rand()));  // Generate a random ClOrdID for simplicity
    FIX::HandlInst handlInst('1');  // Automated execution order, private, no broker intervention
    FIX::Symbol fixSymbol(symbol);
    FIX::Side fixSide(side);
    FIX::TransactTime transactTime;
    FIX::OrderQty orderQty(quantity);
    FIX::OrdType fixOrderType(orderType); // '1' for Market, '2' for Limit

    FIX42::NewOrderSingle newOrder(clOrdID, handlInst, fixSymbol, fixSide, transactTime, fixOrderType);

    if (orderType == FIX::OrdType_LIMIT) {
        FIX::Price fixPrice(price);
        newOrder.set(fixPrice);
    }

    newOrder.set(orderQty);

    try {
        FIX::Session::sendToTarget(newOrder, sessionID);
        std::cout << "Sent NewOrderSingle for " << symbol << std::endl;
    } catch (const FIX::SessionNotFound& e) {
        std::cerr << "Session not found: " << e.what() << std::endl;
    }
}

void FixClient::sendOrderCancelRequest(const std::string& origClOrdID, const std::string& symbol) {
    FIX::OrigClOrdID originalClOrdID(origClOrdID);  // Original ClOrdID
    FIX::ClOrdID clOrdID(std::to_string(rand()));  // New ClOrdID for the cancel request
    FIX::Symbol fixSymbol(symbol);
    FIX::Side fixSide(FIX::Side_BUY);  // Assuming the original order was a buy; adjust accordingly
    FIX::TransactTime transactTime;

    // Correct constructor call with all required parameters
    FIX42::OrderCancelRequest cancelRequest(originalClOrdID, clOrdID, fixSymbol, fixSide, transactTime);

    try {
        FIX::Session::sendToTarget(cancelRequest, sessionID);
        std::cout << "Sent OrderCancelRequest for " << origClOrdID << std::endl;
    } catch (const FIX::SessionNotFound& e) {
        std::cerr << "Session not found: " << e.what() << std::endl;
    }
}

void FixClient::sendOrderCancelReplaceRequest(const std::string& origClOrdID, const std::string& symbol, double newPrice, int newQuantity) {
    FIX::OrigClOrdID originalClOrdID(origClOrdID);  // Original ClOrdID
    FIX::ClOrdID clOrdID(std::to_string(rand()));   // New ClOrdID for the replace request
    FIX::Symbol fixSymbol(symbol);
    FIX::Side fixSide(FIX::Side_BUY);               // Assuming the original order was a buy; adjust accordingly
    FIX::TransactTime transactTime;
    FIX::HandlInst handlInst('1');                  // Automated execution order, private, no broker intervention
    FIX::OrdType ordType(FIX::OrdType_LIMIT);       // Assuming the original order was a limit order; adjust accordingly
    FIX::Price price(newPrice);
    FIX::OrderQty orderQty(newQuantity);

    // Correct constructor call with all required parameters
    FIX42::OrderCancelReplaceRequest replaceRequest(
        originalClOrdID, clOrdID, handlInst, fixSymbol, fixSide, transactTime, ordType
    );

    replaceRequest.set(price);
    replaceRequest.set(orderQty);

    try {
        FIX::Session::sendToTarget(replaceRequest, sessionID);
        std::cout << "Sent OrderCancelReplaceRequest for " << origClOrdID << std::endl;
    } catch (const FIX::SessionNotFound& e) {
        std::cerr << "Session not found: " << e.what() << std::endl;
    }
}

// Handle logon event
void FixClient::onLogon(const FIX::SessionID& sessionID) {
    this->sessionID = sessionID;
    std::cout << "Logon - Session: " << sessionID << std::endl;
}

// Handle logout event
void FixClient::onLogout(const FIX::SessionID& sessionID) {
    std::cout << "Logout - Session: " << sessionID << std::endl;
}

// Handle incoming ExecutionReport messages
void FixClient::onMessage(const FIX42::ExecutionReport& message, const FIX::SessionID& sessionID) {
    try {
        FIX::ClOrdID clOrdID;
        FIX::OrderID orderID;
        FIX::ExecType execType;
        FIX::OrdStatus ordStatus;
        FIX::Symbol symbol;
        FIX::Side side;
        FIX::LeavesQty leavesQty;
        FIX::CumQty cumQty;
        FIX::AvgPx avgPx;
        FIX::Text text;

        message.get(clOrdID);
        message.get(orderID);
        message.get(execType);
        message.get(ordStatus);
        message.get(symbol);
        message.get(side);
        message.get(leavesQty);
        message.get(cumQty);
        message.get(avgPx);
        message.getFieldIfSet(text);

        std::cout << "Received ExecutionReport - "
                  << "ClOrdID: " << clOrdID << ", "
                  << "OrderID: " << orderID << ", "
                  << "ExecType: " << execType << ", "
                  << "OrdStatus: " << ordStatus << ", "
                  << "Symbol: " << symbol << ", "
                  << "Side: " << side << ", "
                  << "LeavesQty: " << leavesQty << ", "
                  << "CumQty: " << cumQty << ", "
                  << "AvgPx: " << avgPx << ", "
                  << "Text: " << text << std::endl;
    } catch (const FIX::FieldNotFound& e) {
        std::cerr << "Field not found: " << e.what() << std::endl;
    }
}

void FixClient::fromApp(const FIX::Message& message, const FIX::SessionID& sessionID)
EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType) {
    crack(message, sessionID);  // Delegate to the appropriate onMessage handler
}
