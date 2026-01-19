#include "TimeManager.hpp"

namespace Axion
{

TimeManager::TimeManager(EngineContext& ctx) : ContextAware(ctx)
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

    frameCount_++;
    fpsTimer_ += deltaTime_;
    if (fpsTimer_ >= 1.0f)
    {
        currentFps_ = frameCount_;
        frameCount_ = 0;
        fpsTimer_ -= 1.0f;
    }
}

} // namespace Axion
