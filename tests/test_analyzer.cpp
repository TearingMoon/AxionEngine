#include <catch2/catch_test_macros.hpp>

#include "axion_engine/managers/analysis/Analyzer.hpp"

TEST_CASE("Analyzer aggregates scoped section timings")
{
    Axion::EngineContext context{};
    Axion::Analyzer analyzer(context);

    analyzer.BeginFrame();
    {
        Axion::ScopedTimer timer(&analyzer, "Physics");
    }
    {
        Axion::ScopedTimer timer(&analyzer, "Physics");
    }
    analyzer.EndFrame();

    const auto* metrics = analyzer.GetSectionMetrics("Physics");

    REQUIRE(metrics != nullptr);
    REQUIRE(metrics->sampleCount == 2);
    REQUIRE(metrics->lastMs >= 0.0f);
    REQUIRE(metrics->averageMs >= 0.0f);
    REQUIRE(metrics->maxMs >= metrics->averageMs);
    REQUIRE(analyzer.GetSectionTime("Physics") == metrics->lastMs);
}
