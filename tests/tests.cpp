#include <iostream>
#include <cassert>
#include <vector>
#include "OrderBook.hpp"

void testAddAndDuplicateOrders() {
    std::cout << "Running testAddAndDuplicateOrders...\n";
    OrderBook book;

    // Test successful add
    assert(book.addOrder({1, Side::Buy, 100, 10}) == true);
    assert(book.addOrder({2, Side::Sell, 105, 5}) == true);

    // Test duplicate ID rejection
    assert(book.addOrder({1, Side::Buy, 101, 20}) == false);
    assert(book.addOrder({2, Side::Sell, 106, 15}) == false);
    
    std::cout << "testAddAndDuplicateOrders PASSED!\n";
}

void testCancelOrders() {
    std::cout << "Running testCancelOrders...\n";
    OrderBook book;

    book.addOrder({10, Side::Buy, 100, 10});

    // Test valid cancel
    assert(book.cancelOrder(10) == true);

    // Test canceling non-existent or already cancelled order
    assert(book.cancelOrder(10) == false);
    assert(book.cancelOrder(999) == false);

    std::cout << "testCancelOrders PASSED!\n";
}

void testBuySideMatchingAndTrades() {
    std::cout << "Running testBuySideMatchingAndTrades...\n";
    OrderBook book;

    // Setup resting sells
    book.addOrder({10, Side::Sell, 10400, 5});
    book.addOrder({11, Side::Sell, 10400, 8});
    book.addOrder({14, Side::Sell, 10500, 20});
    book.addOrder({17, Side::Sell, 10600, 10});

    // Submit incoming buy order that spans multiple levels and partially fills #14
    auto trades = book.submitOrder({50, Side::Buy, 10500, 20});

    // Verify expected trades count
    assert(trades.size() == 3);

    // Trade 1: #50 vs #10 (qty 5 @ 10400)
    assert(trades[0].buyOrderId == 50);
    assert(trades[0].sellOrderId == 10);
    assert(trades[0].price == 10400);
    assert(trades[0].quantity == 5);

    // Trade 2: #50 vs #11 (qty 8 @ 10400)
    assert(trades[1].buyOrderId == 50);
    assert(trades[1].sellOrderId == 11);
    assert(trades[1].price == 10400);
    assert(trades[1].quantity == 8);

    // Trade 3: #50 vs #14 (qty 7 @ 10500)
    assert(trades[2].buyOrderId == 50);
    assert(trades[2].sellOrderId == 14);
    assert(trades[2].price == 10500);
    assert(trades[2].quantity == 7);

    std::cout << "testBuySideMatchingAndTrades PASSED!\n";
}

void testSellSideMatchingAndTrades() {
    std::cout << "Running testSellSideMatchingAndTrades...\n";
    OrderBook book;

    // Setup resting buys (sorted highest first via std::greater)
    book.addOrder({20, Side::Buy, 10200, 5});
    book.addOrder({21, Side::Buy, 10200, 8});
    book.addOrder({24, Side::Buy, 10100, 20});
    book.addOrder({27, Side::Buy, 10000, 10});

    // Submit incoming sell order
    auto trades = book.submitOrder({60, Side::Sell, 10100, 20});

    // Verify expected trades count
    assert(trades.size() == 3);

    // Trade 1: #20 vs #60 (qty 5 @ 10200)
    assert(trades[0].buyOrderId == 20);
    assert(trades[0].sellOrderId == 60);
    assert(trades[0].price == 10200);
    assert(trades[0].quantity == 5);

    // Trade 2: #21 vs #60 (qty 8 @ 10200)
    assert(trades[1].buyOrderId == 21);
    assert(trades[1].sellOrderId == 60);
    assert(trades[1].price == 10200);
    assert(trades[1].quantity == 8);

    // Trade 3: #24 vs #60 (qty 7 @ 10100)
    assert(trades[2].buyOrderId == 24);
    assert(trades[2].sellOrderId == 60);
    assert(trades[2].price == 10100);
    assert(trades[2].quantity == 7);

    std::cout << "testSellSideMatchingAndTrades PASSED!\n";
}

void testPriceLimitNoMatch() {
    std::cout << "Running testPriceLimitNoMatch...\n";
    OrderBook book;

    book.addOrder({10, Side::Sell, 10600, 20});

    // Buy price ($10500) is lower than sell price ($10600) -> should not trade, should rest
    auto trades = book.submitOrder({50, Side::Buy, 10500, 20});
    assert(trades.empty());

    std::cout << "testPriceLimitNoMatch PASSED!\n";
}

int main() {
    std::cout << "=== STARTING AUTOMATED TESTS ===\n";
    testAddAndDuplicateOrders();
    testCancelOrders();
    testBuySideMatchingAndTrades();
    testSellSideMatchingAndTrades();
    testPriceLimitNoMatch();
    std::cout << "=== ALL TESTS PASSED SUCCESSFULLY! ===\n";
    return 0;
}