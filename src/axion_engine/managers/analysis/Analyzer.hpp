#pragma once
#include "axion_engine/structure/ContextAware.hpp"

namespace Axion
{

/**
 * @brief Performance analysis and profiling manager.
 * 
 * Analyzer provides hooks for performance monitoring and profiling.
 * Currently a placeholder for future performance analysis features.
 */
class Analyzer : public ContextAware
{
public:
    Analyzer(EngineContext& ctx);
    ~Analyzer();

    /** @brief Updates analysis metrics. Called each frame. */
    void Update();
};

} // namespace Axion
