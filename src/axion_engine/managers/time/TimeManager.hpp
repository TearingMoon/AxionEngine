#pragma once

#include <stdio.h>
#include <chrono>

#include "axion_engine/structure/ContextAware.hpp"

namespace Axion
{

/**
 * @brief Manages time tracking for the game loop.
 * 
 * TimeManager provides delta time calculation, elapsed time tracking,
 * and FPS measurement. It uses high-resolution clock for accurate timing.
 */
class TimeManager : public ContextAware
{
public:
    TimeManager(EngineContext& ctx);
    ~TimeManager();

    /** @brief Updates time calculations. Called once per frame. */
    void Update();

    /** @brief Returns the time elapsed since the last frame in seconds. */
    float GetDeltaTime() const { return deltaTime_; }
    
    /** @brief Returns the total time elapsed since engine start in seconds. */
    float GetTimeSinceStart() const { return timeSinceStart_; }
    
    /** @brief Returns the current frames per second. */
    int GetFPS() const { return currentFps_; }

private:
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point lastUpdateTime_;

    float deltaTime_ = 0.0f;
    float timeSinceStart_ = 0.0f;

    int frameCount_ = 0;
    float fpsTimer_ = 0.0f;
    int currentFps_ = 0;
};

} // namespace Axion