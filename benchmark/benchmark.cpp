#include "../src/Book.h"
#include <benchmark/benchmark.h>

// 1️⃣ Adding N limit orders
static void BM_AddLimitOrders(benchmark::State& state) {
    int num_orders = state.range(0);

    for (auto _ : state) {
        Book book;
        OrderIdSequence seq;

        for (int i = 0; i < num_orders; ++i) {
            float price = 10.0f + (i % 500) * 0.01f;
            Side side = (i % 2 == 0) ? Side::Buy : Side::Sell;
            int size = 10 + (i % 20);
            book.addOrderToBook(side, size, price, OrderType::Limit, seq);
        }
    }

    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * num_orders);
}
BENCHMARK(BM_AddLimitOrders)
    ->Arg(1'000)
    ->Arg(10'000)
    ->Arg(100'000);

// 2️⃣ Adding + canceling orders
static void BM_AddAndCancelOrders(benchmark::State& state) {
    int num_orders = state.range(0);

    for (auto _ : state) {
        Book book;
        OrderIdSequence seq;

        // Insert orders
        for (int i = 0; i < num_orders; ++i) {
            book.addOrderToBook(Side::Buy, 5, 15.0f + i * 0.01f, OrderType::Limit, seq);
        }

        // Cancel them
        for (int i = 0; i < num_orders; ++i) {
            book.cancelOrder(i);
        }
    }

    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * num_orders * 2);
}
BENCHMARK(BM_AddAndCancelOrders)
    ->Arg(1'000)
    ->Arg(10'000);

// 3️⃣ Adding + modifying orders
static void BM_AddAndModifyOrders(benchmark::State& state) {
    int num_orders = state.range(0);

    for (auto _ : state) {
        Book book;
        OrderIdSequence seq;

        // Insert orders
        for (int i = 0; i < num_orders; ++i) {
            book.addOrderToBook(Side::Sell, 7, 20.0f + i * 0.02f, OrderType::Limit, seq);
        }

        // Modify their prices
        for (int i = 0; i < num_orders; ++i) {
            float new_price = 21.0f + i * 0.02f;
            book.modifyOrderLimitPrice(i, new_price, seq);
        }
    }

    state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * num_orders * 2);
}
BENCHMARK(BM_AddAndModifyOrders)
    ->Arg(1'000)
    ->Arg(10'000);

// 4️⃣ Placing market orders against pre-populated book
static void BM_PlaceMarketOrders(benchmark::State& state) {
    int num_orders = state.range(0);
    int market_order_size = state.range(1);

    for (auto _ : state) {
        Book book;
        OrderIdSequence seq;

        // Populate book
        for (int i = 0; i < num_orders; ++i) {
            book.addOrderToBook(Side::Buy, 5, 25.0f + i * 0.01f, OrderType::Limit, seq);
        }

        // Place market order
        book.placeMarketOrder(market_order_size, Side::Sell);
    }

    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_PlaceMarketOrders)
    ->Args({1'000, 100})
    ->Args({10'000, 1'000})
    ->Args({100'000, 10'000});

// ENTRY POINT
BENCHMARK_MAIN();
