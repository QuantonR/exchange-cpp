#include "Exchange.hpp"

/**
 * @brief Constructs a new Exchange with a specified name.
 * @param exchangeName The name of the exchange.
 */
Exchange::Exchange(const std::string& exchangeName)
    : exchangeName(exchangeName) {}

/**
 * @brief Adds an order to the order book of a specific ticker.
 */
void Exchange::addOrder(const std::string& ticker, Side orderSide, int shares, float limit, OrderType orderType) {
    Book* instrumentBook = getOrderBook(ticker);
    if (!instrumentBook) {
        throw std::runtime_error("Cannot add order: instrument not covered by the exchange.");
    }

    if (orderType == OrderType::Limit) {
        if (limit == -1) {
            throw std::invalid_argument("Limit price must be provided for limit orders.");
        }
        instrumentBook->addOrderToBook(orderSide, shares, limit, orderType, globalOrderId);
    } else if (orderType == OrderType::Market) {
        instrumentBook->placeMarketOrder(shares, orderSide);
    }
}

/**
 * @brief Modifies the limit price of an order.
 */
void Exchange::modifyLimitPrice(const std::string& ticker, int64_t orderId, int newLimitPrice) {
    Book* instrumentBook = getOrderBook(ticker);
    if (!instrumentBook) {
        throw std::runtime_error("Cannot modify price: instrument not covered by the exchange.");
    }
    instrumentBook->modifyOrderLimitPrice(orderId, static_cast<float>(newLimitPrice), globalOrderId);
}

/**
 * @brief Modifies the size of an order.
 */
void Exchange::modifyOrderSize(const std::string& ticker, int64_t orderId, int newSize) {
    Book* instrumentBook = getOrderBook(ticker);
    if (!instrumentBook) {
        throw std::runtime_error("Cannot modify size: instrument not covered by the exchange.");
    }
    instrumentBook->modifyOrderSize(orderId, newSize);
}

/**
 * @brief Adds a new instrument to the exchange.
 */
void Exchange::addInstrument(const std::string& newTicker) {
    tickerLob.emplace(newTicker, std::make_unique<Book>());
}

/**
 * @brief Removes an instrument from the exchange.
 */
void Exchange::removeInstrument(const std::string& ticker) {
    tickerLob.erase(ticker);
}

/**
 * @brief Returns all tickers on the exchange.
 */
std::vector<std::string> Exchange::getTickerList() const {
    std::vector<std::string> tickers;
    for (const auto& pair : tickerLob) {
        tickers.push_back(pair.first);
    }
    return tickers;
}

/**
 * @brief Retrieves the order book for a ticker.
 */
Book* Exchange::getOrderBook(const std::string& ticker) const {
    auto it = tickerLob.find(ticker);
    if (it != tickerLob.end()) {
        return it->second.get();
    }
    return nullptr;
}

/**
 * @brief Retrieves the National Best Bid and Offer (NBBO) for a ticker.
 */
std::pair<std::optional<int>, std::optional<int>> Exchange::getNBBO(const std::string& ticker) const {
    Book* instrumentBook = getOrderBook(ticker);
    if (!instrumentBook) {
        throw std::invalid_argument("Instrument is not covered by the exchange.");
    }

    std::optional<int> bestBid;
    std::optional<int> bestOffer;

    const Limit* bestBidLimit = instrumentBook->getBuySide().getBestLimit();
    if (bestBidLimit) {
        bestBid = bestBidLimit->getLimitPrice();
    }

    const Limit* bestOfferLimit = instrumentBook->getSellSide().getBestLimit();
    if (bestOfferLimit) {
        bestOffer = bestOfferLimit->getLimitPrice();
    }

    return {bestBid, bestOffer};
}
