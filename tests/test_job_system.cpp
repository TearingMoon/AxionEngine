#include <atomic>

#include <catch2/catch_test_macros.hpp>

#include "axion_engine/core/JobSystem.hpp"

TEST_CASE("JobSystem executes submitted work", "[jobs]")
{
    Axion::JobSystem jobs(2);
    auto result = jobs.Submit([]() { return 42; });
    REQUIRE(result.get() == 42);
}

TEST_CASE("JobSystem parallel-for covers every index", "[jobs]")
{
    Axion::JobSystem jobs(2);
    std::atomic<int> total{0};
    jobs.ParallelFor(100, [&total](std::size_t) {
        total.fetch_add(1, std::memory_order_relaxed);
    });
    REQUIRE(total.load() == 100);
}
