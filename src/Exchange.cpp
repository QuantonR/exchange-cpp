#include "Exchange.hpp"

Exchange::Exchange(const std::string& exchangeName) : exchangeName(exchangeName) {}

/**
 * @brief Adds an order to the order book of a specific ticker.
 * @param ticker The ticker symbol of the stock.
 * @param orderSide The side of the order (true for buy, false for sell).
 * @param orderVolume The volume of the order.
 * @param orderType The type of the order (limit or market).
 * @param floatLimitPrice The limit price of the order (optional, only for limit orders).
 */
void Exchange::addOrder(const std::string& ticker, bool orderSide, int orderVolume, OrderType orderType, std::optional<float> floatLimitPrice) {
    
    Book* instrumentBook = getOrderBook(ticker);
    
    if (instrumentBook){
        if (orderType == OrderType::GoodTillCancel){
            if (!floatLimitPrice.has_value()) {
                throw std::invalid_argument("Limit price must be provided for limit orders.");
            }
            
            instrumentBook->addOrderToBook(orderSide, orderVolume, floatLimitPrice.value());
        } else if (orderType == OrderType::Market){
            
            instrumentBook->placeMarketOrder(orderVolume, orderSide);
        }
    } else {
        throw std::runtime_error("Can't add order to Exchange. The insturment is not covered by the exchange.");
    }
}

/**
 * @brief Modifies the limit price of an order.
 * @param ticker The ticker symbol of the stock.
 * @param orderId The ID of the order to be modified.
 * @param newLimitPrice The new limit price.
 */
void Exchange::modifyLimitPrice(const std::string& ticker, int64_t orderId, int newLimitPrice) {
    
    Book* instrumentBook = getOrderBook(ticker);
    instrumentBook->modifyOrderLimitPrice(orderId, newLimitPrice);
}

/**
 * @brief Modifies the size of an order.
 * @param ticker The ticker symbol of the stock.
 * @param orderId The ID of the order to be modified.
 * @param newSize The new size of the order.
 */
void Exchange::modifyOrderSize(const std::string& ticker, int64_t orderId, int newSize) {
    
    Book* instrumentBook = getOrderBook(ticker);
    instrumentBook->modifyOrderSize(orderId, newSize);
}

/**
 * @brief Adds a new ticker to the exchange.
 * @param newTicker The ticker symbol of the new stock.
 */
void Exchange::addInstrument(const std::string& newTicker) {
    
    tickerLob.emplace(newTicker, std::make_unique<Book>());
}

/**
 * @brief Removes an existing ticker from the exchange.
 * @param ticker The ticker symbol of the stock to be removed.
 */
void Exchange::removeInstrument(const std::string& ticker){
    
    tickerLob.erase(ticker);
}


/**
 * @brief Retrieves a list of all tickers currently traded on the exchange.
 * @return A vector of ticker symbols.
 */
std::vector<std::string> Exchange::getTickerList() const {
    
    std::vector<std::string> tickers;
    for (const auto& pair : tickerLob) {
        tickers.push_back(pair.first);
    }
    return tickers;
}

/**
 * @brief Returns the book given a ticker.
 * @param ticker The ticker symbol of a particular stock.
 */
Book* Exchange::getOrderBook(const std::string& ticker) const {
    
    auto it = tickerLob.find(ticker);
    if (it != tickerLob.end()) {
        return it->second.get();
    }
    return nullptr;
}

/**
 * @brief Returns National Best Bid and Offer for an Instrument
 * @param ticker The ticker symbol of a particular stock.
 * @return A pair containing the best bid and best offer
 */
std::pair<std::optional<int>, std::optional<int>> Exchange::getNBBO(const std::string& ticker) const {
    
    Book* instrumentBook = getOrderBook(ticker);
    
    if (instrumentBook == nullptr){
        
        throw std::invalid_argument("The insturment is not covered by the exchange.");
    }
    std::optional<int> bestBid;
    std::optional<int> bestOffer;
    
    if (const Limit* bestBidLimit = instrumentBook->getBuySide()->getBestLimit()) {
        bestBid = bestBidLimit->getLimitPrice();
    } else {
        bestBid = std::nullopt;
    }

    if (const Limit* bestOfferLimit = instrumentBook->getSellSide()->getBestLimit()) {
        bestOffer = bestOfferLimit->getLimitPrice();
    } else {
        bestOffer = std::nullopt;
    }

    return {bestBid, bestOffer};
}
