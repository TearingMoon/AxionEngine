#pragma once

#include <stdio.h>
#include <chrono>

#include "axion_engine/structure/ContextAware.hpp"

class TimeManager : public ContextAware
{
public:
    TimeManager(EngineContext &ctx) ;
    ~TimeManager();

    void Update();

    float GetDeltaTime() const { return deltaTime_; }
    float GetTimeSinceStart() const { return timeSinceStart_; }
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