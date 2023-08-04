#include <iostream>
#include <string>
#include <vector>
#include "OrderBookEntry.h"
#include "MerkelMain.h"
#include "CSVReader.h"
#include "Wallet.h"

int main()
{   
    MerkelMain app{};
    app.init();
    
    // Wallet wallet;
    // wallet.insertCurrency("BTC", 10);
    // wallet.insertCurrency("USTD", 100);
    // std::cout << wallet.toString() << std::endl;
    // wallet.removeCurrency("USTD", 10);
    // std::cout << wallet.toString() << std::endl;
}
