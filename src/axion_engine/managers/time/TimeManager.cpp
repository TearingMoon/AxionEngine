#include "TimeManager.hpp"

TimeManager::TimeManager(EngineContext &ctx) : ContextAware(ctx)
{
    lastUpdateTime_ = Clock::now();
    INFO("TimeManager initialized.");
}

TimeManager::~TimeManager()
{
    INFO("TimeManager destroyed.");
}

void TimeManager::Update()
{
    auto now = Clock::now();
    std::chrono::duration<float> frameDuration = now - lastUpdateTime_;
    lastUpdateTime_ = now;

    
    deltaTime_ = frameDuration.count();
    timeSinceStart_ += deltaTime_;

    // FPS Calculation
    frameCount_++;
    fpsTimer_ += deltaTime_;
    if (fpsTimer_ >= 1.0f) // Update FPS meta every second
    {
        currentFps_ = frameCount_;
        frameCount_ = 0;
        fpsTimer_ -= 1.0f;
    }
}
