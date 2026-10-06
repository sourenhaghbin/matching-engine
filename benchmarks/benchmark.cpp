#include <iostream>
#include <chrono>
#include "OrderBook.hpp"

int main() {
    OrderBook book;
    const int numOrders = 100000;

    std::cout << "Starting benchmark with " << numOrders << " orders...\n";

    auto start = std::chrono::high_resolution_clock::now();

    // Populate the book with a mix of resting sells and buying/selling pressure
    for (int i = 1; i <= numOrders; ++i) {
        Side side = (i % 2 == 0) ? Side::Buy : Side::Sell;
        Price price = 10000 + (i % 500); // Prices fluctuate between 10000 and 10500
        Quantity qty = 10;
        
        book.submitOrder({static_cast<OrderId>(i), side, price, qty});
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cout << "Benchmark completed!\n";
    std::cout << "Total time: " << elapsed.count() << " ms\n";
    std::cout << "Average time per order: " << (elapsed.count() / numOrders) * 1000.0 << " microseconds\n";

    return 0;
}