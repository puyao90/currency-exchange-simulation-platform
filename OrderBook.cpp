#include"OrderBook.h"
#include "CSVReader.h"
#include <map>
#include "Candlestick.h"

OrderBook::OrderBook(std::string filename){
    orders = CSVReader::readCSV(filename);
};

std::vector<std::string> OrderBook::getKnowProducts(){
    std::vector<std::string> products;
    std::map<std::string, bool> prodMap;
    for(OrderBookEntry& e :orders){
        prodMap[e.product] = true;
    }
    for(auto const& e:prodMap){
        products.push_back(e.first);
    }
    return products;
};

std::vector<std::string> OrderBook::getTimestampsBefore(OrderBookType type,
    std::string product, std::string timestamp){
    std::vector<std::string> timestamps,timestampsBefore;
    std::vector<OrderBookEntry> orders_sub;
    for(OrderBookEntry& e:orders){
        if(e.orderType==type&&e.product==product){
            orders_sub.push_back(e);
        }
    }
    std::map<std::string, bool> timeMap;
    for(auto const& e:orders_sub){
        timeMap[e.timestamp] = true;
    }
    for(auto const& e:timeMap){
        timestamps.push_back(e.first);
    }
    for(auto const& e:timestamps){
        if(e<=timestamp){
        timestampsBefore.push_back(e);
        }
    } 
    return timestampsBefore;
};

std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type,std::string product,std::string timestamp){
    std::vector<OrderBookEntry> orders_sub;
    for(OrderBookEntry& e:orders){
        if(e.orderType==type&&e.product==product&&e.timestamp==timestamp){
            orders_sub.push_back(e);
        }
    }
    return orders_sub;
};

Candlestick OrderBook::generateCandle(OrderBookType type,std::string product,std::string timestamp){
    double high,low,open,close;
    std::vector<OrderBookEntry> entries = getOrders(type, product, timestamp);
    high = getHighPrice(entries);
    low = getLowPrice(entries);
    close = getAveragePrice(entries);
    std::string lasttime = getLastTime(timestamp);
    std::vector<OrderBookEntry> lasttime_entries = getOrders(type, product, lasttime);
    if(lasttime==""){
        open = close;
    }
    else
    {
        open = getAveragePrice(lasttime_entries);
    }
    Candlestick candlestick{timestamp, open, close, high, low};
    return candlestick;
}

 std::vector<Candlestick> OrderBook::candlestickData(OrderBookType type,std::string product,
    std::string timestamp){
    std::vector<Candlestick> candlestickData;
    std::vector<std::string> timestampsbefore = getTimestampsBefore(type, product, timestamp);
    for (const auto& timestamp : timestampsbefore) {
            Candlestick candlestick = generateCandle(type,product,timestamp);
            candlestickData.push_back(candlestick);
    }
    return candlestickData;
 }

double OrderBook::getHighPrice(std::vector<OrderBookEntry>& orders){
    double max = orders[0].price;
    for(OrderBookEntry&e:orders){
        if(e.price>max)max = e.price;
    }
    return max;
}

double OrderBook::getLowPrice(std::vector<OrderBookEntry>& orders){
    double min = orders[0].price;
    for(OrderBookEntry&e:orders){
        if(e.price<min)min = e.price;
    }
    return min;
}

double OrderBook::getMeanPrice(std::vector<OrderBookEntry>& orders){
    double total=0;
    double mean;
    for (unsigned int i = 0;i<orders.size();++i)
    {
        total += orders[i].price;
    }
    mean = total / orders.size();
    return mean;
}

double OrderBook::getAveragePrice(std::vector<OrderBookEntry>& orders){
    double totalvalue=0;
    double totalamount=0;
    double average;
    for (unsigned int i = 0;i<orders.size();++i)
    {
        totalvalue += orders[i].price * orders[i].amount;
        totalamount += orders[i].amount;
    }
    average = totalvalue / totalamount;
    return average;
}

std::string OrderBook::getEarliestTime(){
    return orders[0].timestamp;
}

std::string OrderBook::getLastTime(std::string timestamp){
    std::string previous = "";
    std::string current = "";
    if(timestamp==orders[0].timestamp){
        previous = "";
    }
    else
    {
        for (OrderBookEntry &e : orders)
        {
            previous = current;
            current = e.timestamp;
            if (current >= timestamp)
            {
                break;
            }
        }
    }
    return previous;
}

std::string OrderBook::getNextTime(std::string timestamp){
    std::string nextTime = "";
    for (OrderBookEntry &e : orders)
    {
        if(e.timestamp>timestamp){
            nextTime = e.timestamp;
            break;
        }
    }
    if(nextTime==""){
        nextTime = orders[0].timestamp;
    }
    return nextTime;
}

void OrderBook::insertOrder(OrderBookEntry& order){
    orders.push_back(order);
    std::sort(orders.begin(), orders.end(), OrderBookEntry::compareByTimestamp);
}

std::vector<OrderBookEntry> OrderBook::matchAsksToBids(std::string product, std::string timestamp){
    std::vector<OrderBookEntry> asks = getOrders(OrderBookType::ask, product, timestamp);
    std::vector<OrderBookEntry> bids = getOrders(OrderBookType::bid, product, timestamp);

    std::vector<OrderBookEntry> sales;
    std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
    std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);

    for(OrderBookEntry& a : asks){
        for(OrderBookEntry& b : bids){
            if(b.price>=a.price){
                OrderBookEntry sale{a.price, 0, timestamp, product, OrderBookType::asksale}; 
                if(b.username=="simuser"){
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::bidsale;
                }
                if(a.username=="simuser"){
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::asksale;
                }

                if(b.amount==a.amount){
                    sale.amount = a.amount;
                    sales.push_back(sale);
                    b.amount = 0;
                    break;
                }
                if(b.amount>a.amount){
                    sale.amount = a.amount;
                    sales.push_back(sale);
                    b.amount = b.amount - a.amount;
                    break;
                }
                if(b.amount<a.amount && b.amount>0){
                    sale.amount = b.amount;
                    sales.push_back(sale);
                    a.amount = a.amount - b.amount;
                    b.amount = 0;
                    continue;
                }
            }
        }
    }
    return sales;
}