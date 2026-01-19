#pragma once

namespace Axion
{

/**
 * @brief Configuration parameters for the game engine.
 * 
 * This structure holds all the initialization settings that control
 * the engine's behavior at startup and during runtime.
 */
struct EngineConfig
{
    /** @brief Frame delay in milliseconds between update cycles. Default is 1ms. */
    int updateDelay = 1;

    /** @brief Maximum number of log files to keep before cleanup. Default is 5. */
    std::size_t maxLogFiles = 5;
};

} // namespace Axion