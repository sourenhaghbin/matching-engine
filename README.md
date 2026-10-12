# High-Performance Limit Order Book & Matching Engine

A C++20 limit order book and matching engine that simulates a continuous double auction with **price-time priority**. It supports limit order submission, full and partial fills, cancellation, and structured trade reporting.

## Features

- **Price-time priority:** Best price first; FIFO within each price level.
- **BUY and SELL matching:** Full fills, partial fills, and execution across multiple resting orders.
- **Cancellation:** Hash-based active-order lookup with stable list iterators.
- **Trade reporting:** Buyer ID, seller ID, execution price, and quantity for each match.
- **Order ID validation:** Rejects duplicate active IDs; permits reuse after an order leaves the book.
- **Correctness tests:** Covers matching, non-crossing orders, FIFO behavior, and cancellation.

## Example

Initial resting sell orders:

| Price | Order ID | Quantity |
| --- | --- | ---: |
| $104.00 | 10 | 5 |
| $104.00 | 11 | 8 |
| $105.00 | 14 | 20 |
| $106.00 | 17 | 10 |

Submit **BUY #50: 20 units, limit $105.00**.

```text
[TRADE] Buy #50 matched Sell #10 | Qty: 5 @ $104.00
[TRADE] Buy #50 matched Sell #11 | Qty: 8 @ $104.00
[TRADE] Buy #50 matched Sell #14 | Qty: 7 @ $105.00
```

The incoming order is fully filled. Orders #10 and #11 are removed, and resting sell #14 retains 13 units. Trades execute at the **resting order's price**.

## Data Structures

```cpp
std::map<Price, std::list<Order>, std::greater<>> buyOrders;
std::map<Price, std::list<Order>> sellOrders;

struct OrderLocation {
    Side side;
    Price price;
    std::list<Order>::iterator itr;
};

std::unordered_map<OrderId, OrderLocation> orderLookup;
```

- The highest bid and lowest ask are accessible via their respective maps' `begin()` iterators.
- Each price level stores orders in insertion order using `std::list`.
- The hash index identifies an active order's side, price level, and list position, enabling efficient cancellation without scanning the book.
- Prices use integer units (cents in the examples), avoiding floating-point comparisons.

### Complexity

Let **P** be the number of active price levels.

| Operation | Complexity |
| --- | --- |
| Order ID lookup | Average O(1) |
| Price-level lookup | O(log P) |
| Insert into a price level | O(log P) overall |
| Erase an order using its list iterator | O(1) |
| Cancel an order, including price-level lookup | O(log P) |
| Match an incoming order | Depends on orders and price levels consumed |

## Preliminary Benchmark

A single synthetic benchmark submitted 100,000 sequential orders, alternating BUY and SELL sides, with prices varying over a fixed range.

| Metric | Result |
| --- | ---: |
| Orders submitted | 100,000 |
| Elapsed time | 5.8398 ms |
| Throughput | ~17.1 million orders/second |
| Amortized time per order | ~58.4 ns |

**Important:** These are preliminary, single-run throughput results, not independently measured per-order latency. Performance depends on workload, hardware, compiler, and optimization flags. Larger, repeated benchmarks are planned.

## Build and Run

Requires a C++20-compatible compiler. From the project root:

```bash
g++ -std=c++20 -O2 -Iinclude src/main.cpp src/OrderBook.cpp -o matching_engine
./matching_engine
```

On Windows with MSYS2 UCRT64 and PowerShell:

```powershell
C:\msys64\ucrt64\bin\g++.exe -std=c++20 -O2 -Iinclude src/main.cpp src/OrderBook.cpp -o matching_engine.exe
.\matching_engine.exe
```

If your repository includes a configured `CMakeLists.txt`, you can instead build with:

```bash
cmake -S . -B build
cmake --build build
```

## Testing

Correctness scenarios include:

- Full and partial fills on both sides
- FIFO execution at a shared price
- Matching across multiple price levels
- Non-crossing orders resting on the book
- Cancellation of active and nonexistent orders
- Duplicate active IDs and ID reuse after cancellation

The matching logic is single-threaded and deterministic. Tests should check both generated trades and the resulting order book state.

## Future Work

- Repeatable benchmarks with larger and varied workloads
- Latency distributions and profiling
- Allocation and cache-locality optimizations
- Market data snapshots and networking interfaces
- Additional order types and multiple instruments

## Status

Core limit-order matching, cancellation, and trade reporting are implemented. The next focus is stronger automated verification and reproducible performance benchmarking.
