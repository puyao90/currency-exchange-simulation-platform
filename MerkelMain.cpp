#include "MerkelMain.h"
#include <iostream>
#include <vector>
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "Candlestick.h"
#include "Chart.h"

MerkelMain::MerkelMain()
{
}

void MerkelMain::init()
{
    int input;
    currentTime = orderbook.getEarliestTime();

    wallet.insertCurrency("BTC", 10);

    while (true)
    {
        printMenu();
        input = getUserOption();
        processUserOption(input);
    }
}

void MerkelMain::printMenu()
{
    // 1 print help
    std::cout << "1: Print help " << std::endl;
    // 2 print exchange stats
    std::cout << "2: Print market stats" << std::endl;
    // 3 print exchange stats
    std::cout << "3: Print candlestick data (until now)" << std::endl;
    // 4 make an offer
    std::cout << "4: Make an offer " << std::endl;
    // 5 make a bid 
    std::cout << "5: Make a bid " << std::endl;
    // 6 print wallet
    std::cout << "6: Print wallet " << std::endl;
    // 7 continue   
    std::cout << "7: Continue " << std::endl;

    std::cout << "============== " << std::endl;
    std::cout << "current time is: " << currentTime << std::endl;
}

void MerkelMain::printHelp()
{
    std::cout << "Help - your aim is to make money. Analyse the market and make bids and offers. " << std::endl;
}

void MerkelMain::printMarketStats()
{
    for(std::string const p:orderbook.getKnowProducts()){
        std::cout << "Products:" << p << std::endl;
        std::vector<OrderBookEntry> entries = orderbook.getOrders(OrderBookType::ask,p,currentTime);
        std::cout << "Asks seen:" <<entries.size() << std::endl;
        std::cout << "Max ask:" <<OrderBook::getHighPrice(entries) << std::endl;
        std::cout << "Min ask:" <<OrderBook::getLowPrice(entries) << std::endl;
        std::cout << "Mean ask:" <<OrderBook::getMeanPrice(entries) << std::endl;
        generateScatterChart(entries);
    }
}

void MerkelMain::printCandlestickDate()
{
    for(std::string const p:orderbook.getKnowProducts()){
        std::cout << "Products:" << p << std::endl;
        std::vector<Candlestick> candlestickData = orderbook.candlestickData(
            OrderBookType::ask, p, currentTime);
        for (const auto& e : candlestickData) {
            std::cout <<"timestamp:"<< e.timestamp << " open:"<<e.open<< " close:"<< 
            e.close<< " high:"<<e.high<< " low:"<<e.low<<'\n';
            
        }
        generateCandlestickChart(candlestickData);
    }
}

void MerkelMain::enterAsk()
{
    std::cout << "Make an ask - enter the amount: product, price, amount, eg ETH/BTC,200,0.5" << std::endl;
    // std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    std::string input;
    std::getline(std::cin, input);
    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if(tokens.size() !=3){
        std::cout << "MerkelMain::enterAsk Bad input!" << input << std::endl;
    }
    else{
        try{
            OrderBookEntry obe = CSVReader::stringsToOBE(tokens[1],tokens[2],currentTime,tokens[0],OrderBookType::ask);
            obe.username = "simuser";
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good" << std::endl;
                orderbook.insertOrder(obe);
            }
            else
            {
                std::cout << "Wallet has insufficient funds" << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::cout << "MerkelMain::enterAsk Bad input!" << std::endl;
        }
    }

    std::cout << "You typed:" << input << std::endl;
}

void MerkelMain::enterBid()
{
    std::cout << "Make an bid - enter the amount: product, price, amount, eg ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);
    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if(tokens.size() !=3){
        std::cout << "MerkelMain::enterBid Bad input!" << input << std::endl;
    }
    else{
        try{
            OrderBookEntry obe = CSVReader::stringsToOBE(tokens[1],tokens[2],currentTime,tokens[0],OrderBookType::bid);
            obe.username = "simuser";
            if(wallet.canFulfillOrder(obe)){
                std::cout << "Wallet looks good" << std::endl;
                orderbook.insertOrder(obe);
            }else{
                std::cout << "Wallet has insufficient funds" << std::endl;
            }        
        }
        catch (const std::exception &e)
        {
            std::cout << "MerkelMain::enterBid Bad input!" << std::endl;
        }
    }

    std::cout << "You typed:" << input << std::endl;
}

void MerkelMain::printWallet()
{
   std::cout << wallet.toString() << std::endl;
}
        
void MerkelMain::gotoNextTimeframe()
{
    std::cout << "Going to next time frame. " << std::endl;
    for(std::string p : orderbook.getKnowProducts()){
        std::cout << "matching" << p <<std::endl;
        std::vector<OrderBookEntry> sales = orderbook.matchAsksToBids(p,currentTime);
        std::cout << "Sales size:" <<sales.size()<<std::endl;
        for(OrderBookEntry& sale: sales){
            std::cout << "Sale price:" << sale.price << "  Sale amount:" << sale.amount <<std::endl;
            if(sale.username=="simuser"){
                wallet.processSale(sale);
            }
        }
    }
    currentTime = orderbook.getNextTime(currentTime);
}

int MerkelMain::getUserOption()
{
    int userOption=0;
    std::string line;
    std::cout << "Type in 1-6" << std::endl;
    std::getline(std::cin, line);
    try{
        userOption = std::stoi(line);
    }catch(const std::exception& e){
    }
    
    std::cout << "You chose: " << userOption << std::endl;
    return userOption;
}

void MerkelMain::processUserOption(int userOption)
{
    if (userOption == 0) // bad input
    {
        std::cout << "Invalid choice. Choose 1-6" << std::endl;
    }
    if (userOption == 1) 
    {
        printHelp();
    }
    if (userOption == 2) 
    {
        printMarketStats();
    }
    if (userOption == 3) 
    {
        printCandlestickDate();
    }
    if (userOption == 4) 
    {
        enterAsk();
    }
    if (userOption == 5) 
    {
        enterBid();
    }
    if (userOption == 6) 
    {
        printWallet();
    }
    if (userOption == 7) 
    {
        gotoNextTimeframe();
    }       
}
