#pragma once
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "Candlestick.h"

class OrderBook{
    public:
    OrderBook(std::string filename);
    std::vector<std::string> getKnowProducts();
    std::vector<OrderBookEntry> getOrders(OrderBookType type,std::string product,std::string timestamp);

    // To obtain all timestamps of a specified type of product in all orders before a certain timestamp, 
    // in order to derive candlestick data for all dates before that date.
    std::vector<std::string> getTimestampsBefore(OrderBookType type,std::string product,std::string timestamp);
    
    Candlestick generateCandle(OrderBookType type, std::string product, std::string timestamp);
    std::vector<Candlestick> candlestickData(OrderBookType type, std::string product, std::string timestamp);

    std::string getEarliestTime();
    std::string getLastTime(std::string timestamp);
    std::string getNextTime(std::string timestamp);

    void insertOrder(OrderBookEntry& order);

    std::vector<OrderBookEntry> matchAsksToBids(std::string product, std::string timestamp);

    static double getHighPrice(std::vector<OrderBookEntry> &orders);
    static double getLowPrice(std::vector<OrderBookEntry>& orders);
    static double getMeanPrice(std::vector<OrderBookEntry>& orders);
    double getAveragePrice(std::vector<OrderBookEntry> &orders);

private:
    std::vector<OrderBookEntry> orders;
};