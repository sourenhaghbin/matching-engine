#include <iostream>
#include "OrderBook.hpp"

int main() {
    OrderBook book;

    book.addOrder({10, Side::Sell, 10400, 5});
    book.addOrder({11, Side::Sell, 10400, 8});
    book.addOrder({14, Side::Sell, 10500, 20});
    book.addOrder({17, Side::Sell, 10600, 10});

    std::cout << "Submitting order #50 BUY 20 @ 10500...\n";
    auto trades = book.submitOrder({50, Side::Buy, 10500, 20});

    std::cout << "\nExecuted Trades (" << trades.size() << "):\n";
    for (const auto& t : trades) {
        std::cout << "  [TRADE] Buy #" << t.buyOrderId 
                  << " matched Sell #" << t.sellOrderId 
                  << " | Qty: " << t.quantity 
                  << " @ $" << t.price << "\n";
    }

    std::cout << "\nBook state after trades:\n";
    book.print();

    return 0;
}