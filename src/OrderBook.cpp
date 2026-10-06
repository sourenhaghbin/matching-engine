
#include "Order.hpp"
#include "OrderBook.hpp"
#include <map>
#include <unordered_map>
#include <list>
#include <iterator>
#include <iostream>

bool OrderBook::addOrder (const Order& currOrder){

    if (orderLookup.contains(currOrder.id)) return false;

    OrderLocation location;

    if (currOrder.side==Side::Buy){
        auto& orders = buyOrders[currOrder.price];
        orders.push_back(currOrder);
        location.side=Side::Buy;
        location.itr=std::prev(orders.end());
    } else {
        auto& orders = sellOrders[currOrder.price];
        orders.push_back(currOrder);
        location.side=Side::Sell;
        location.itr=std::prev(orders.end());
    }
   
    location.price=currOrder.price;

    orderLookup.insert({currOrder.id, location});

    return true;
}

bool OrderBook::cancelOrder (OrderId id){

    // Find the iterator of id in order lookup.
    auto lookupIt = orderLookup.find(id);

    // Order ID does not exist.
    if (lookupIt == orderLookup.end()){
        return false;
    } 

    const OrderLocation& location = lookupIt->second;

    if (location.side==Side::Buy){

        // Find the associated price level.
        auto mapIt = buyOrders.find(location.price);
        mapIt->second.erase(location.itr);
        if (mapIt->second.empty()){
            buyOrders.erase(mapIt);
        }
        

    } else {

        // Find the associated price level.
        auto mapIt = sellOrders.find(location.price);
        mapIt->second.erase(location.itr);
        if (mapIt->second.empty()){
            sellOrders.erase(mapIt);
        }

    }

    orderLookup.erase(lookupIt);

    return true;
}

std::vector<Trade> OrderBook::submitOrder(Order order) {
    std::vector<Trade> trades;
    if (order.quantity == 0) return trades;

    if (order.side == Side::Buy) {
        auto it = sellOrders.begin();
        while (it != sellOrders.end() && order.quantity > 0) {
            Price price = it->first;
            if (price > order.price) break;

            auto& restingOrders = it->second;
            auto listIt = restingOrders.begin();

            while (listIt != restingOrders.end() && order.quantity > 0) {
                Quantity tradedQty = std::min(order.quantity, listIt->quantity);

                // Record the trade (Incoming = Buy, Resting = Sell)
                trades.push_back({
                    /* .buyOrderId  = */ order.id,
                    /* .sellOrderId = */ listIt->id,
                    /* .price       = */ price,
                    /* .quantity    = */ tradedQty
                });

                if (order.quantity >= listIt->quantity) {
                    order.quantity -= listIt->quantity;
                    orderLookup.erase(listIt->id);
                    listIt = restingOrders.erase(listIt);
                } else {
                    listIt->quantity -= order.quantity;
                    order.quantity = 0;
                    break;
                }
            }

            if (restingOrders.empty()) {
                it = sellOrders.erase(it);
            } else {
                ++it;
            }
        }

        if (order.quantity > 0) {
            addOrder(order);
        }

    } else { // Side::Sell
        auto it = buyOrders.begin();
        while (it != buyOrders.end() && order.quantity > 0) {
            Price price = it->first;
            if (price < order.price) break;

            auto& restingOrders = it->second;
            auto listIt = restingOrders.begin();

            while (listIt != restingOrders.end() && order.quantity > 0) {
                Quantity tradedQty = std::min(order.quantity, listIt->quantity);

                // Record the trade (Resting = Buy, Incoming = Sell)
                trades.push_back({
                    /* .buyOrderId  = */ listIt->id,
                    /* .sellOrderId = */ order.id,
                    /* .price       = */ price,
                    /* .quantity    = */ tradedQty
                });

                if (order.quantity >= listIt->quantity) {
                    order.quantity -= listIt->quantity;
                    orderLookup.erase(listIt->id);
                    listIt = restingOrders.erase(listIt);
                } else {
                    listIt->quantity -= order.quantity;
                    order.quantity = 0;
                    break;
                }
            }

            if (restingOrders.empty()) {
                it = buyOrders.erase(it);
            } else {
                ++it;
            }
        }

        if (order.quantity > 0) {
            addOrder(order);
        }
    }

    return trades;
}

void OrderBook::print() const {
    std::cout << "===========================\n";
    std::cout << "------- ORDER BOOK --------\n";
    
    std::cout << "SELL ORDERS (Asks):\n";
    if (sellOrders.empty()) {
        std::cout << "  (none)\n";
    } else {
        for (const auto& [price, orders] : sellOrders) {
            for (const auto& order : orders) {
                std::cout << "  $" << price << " -> ID: " << order.id << " | Qty: " << order.quantity << "\n";
            }
        }
    }

    std::cout << "---------------------------\n";

    std::cout << "BUY ORDERS (Bids):\n";
    if (buyOrders.empty()) {
        std::cout << "  (none)\n";
    } else {
        for (const auto& [price, orders] : buyOrders) {
            for (const auto& order : orders) {
                std::cout << "  $" << price << " -> ID: " << order.id << " | Qty: " << order.quantity << "\n";
            }
        }
    }
    
    std::cout << "---------------------------\n";
    std::cout << "===========================\n\n";
}

    

