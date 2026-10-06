#pragma once

#include "Order.hpp"
#include "Trade.hpp"
#include <map>
#include <unordered_map>
#include <list>
#include <vector>

struct OrderLocation {
    Side side;
    Price price;
    std::list<Order>::iterator itr;
};

class OrderBook {
public:

    bool addOrder(const Order& order);
    bool cancelOrder(OrderId id);
    std::vector<Trade> submitOrder(Order order);
    void print() const;

private:

    std::map<Price, std::list<Order>, std::greater<>> buyOrders;
    std::map<Price, std::list<Order>> sellOrders;
    std::unordered_map<OrderId, OrderLocation> orderLookup;

};




