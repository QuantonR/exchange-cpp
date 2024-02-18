#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <sstream>


int retrieveBitcoinOrderBook();

int main() {
    std::cout << retrieveBitcoinOrderBook() << std::endl;
    return 0;
}

int retrieveBitcoinOrderBook(){

    const std::string apiKey = "0B2BC73A-3FD0-4A1A-8980-495716F16749";
    const std::string url = "https://rest.coinapi.io/v1/orderbooks/current?symbol=KRAKEN_SPOT_BTC_USD&apikey=" + apiKey;

};
