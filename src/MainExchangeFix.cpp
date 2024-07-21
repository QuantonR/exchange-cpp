#include <quickfix/SocketAcceptor.h>
#include "ExchangeFixApplication.hpp"


int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " CONFIG_FILE" << std::endl;
        return 1;
    }

    std::string configFile = argv[1];
    Exchange exchange("MyExchange");

    try {
        ExchangeFixApplication application(exchange);
        FIX::SessionSettings settings(configFile);
        FIX::FileStoreFactory storeFactory(settings);
        FIX::ScreenLogFactory logFactory(settings);
        FIX::SocketAcceptor acceptor(application, storeFactory, settings, logFactory);

        acceptor.start();
        std::cout << "Exchange running, press <enter> to quit" << std::endl;
        std::cin.get();
        acceptor.stop();
    } catch (const FIX::ConfigError& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
