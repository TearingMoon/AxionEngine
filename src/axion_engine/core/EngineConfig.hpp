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
    /**
     * @brief Maximum render/update frequency. Zero disables the limiter.
     *
     * The limiter sleeps only for the remaining frame budget, so systems
     * still run as fast as possible up to this frequency.
     */
    int maxFps = 0;
    bool vsync = false;
    float fixedPhysicsTimestep = 1.0f / 60.0f;
    int maxPhysicsStepsPerFrame = 8;

    /**
     * @brief Legacy fixed delay in milliseconds.
     *
     * Prefer maxFps. This remains for source compatibility and is only used
     * when maxFps is zero.
     */
    int updateDelay = 0;

    /** @brief Maximum number of log files to keep before cleanup. Default is 5. */
    std::size_t maxLogFiles = 5;
};

} // namespace Axion