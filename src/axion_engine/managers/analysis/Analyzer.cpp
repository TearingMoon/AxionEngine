#include "Analyzer.hpp"

#include <algorithm>
#include <numeric>
#include <cmath>

#include "axion_engine/core/EngineContext.hpp"
#include "axion_engine/managers/scene/SceneManager.hpp"
#include "axion_engine/runtime/classes/scene/Scene.hpp"
#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"

namespace Axion
{

// ============================================================================
// ScopedTimer Implementation
// ============================================================================

ScopedTimer::ScopedTimer(Analyzer* analyzer, const std::string& name)
    : analyzer_(analyzer)
    , name_(name)
    , start_(std::chrono::high_resolution_clock::now())
{
}

ScopedTimer::~ScopedTimer()
{
    if (analyzer_)
    {
        auto end = std::chrono::high_resolution_clock::now();
        float ms = std::chrono::duration<float, std::milli>(end - start_).count();
        analyzer_->RecordSectionTime(name_, ms);
    }
}

// ============================================================================
// Analyzer Implementation
// ============================================================================

Analyzer::Analyzer(EngineContext& ctx) : ContextAware(ctx)
{
    lastFrameEnd_ = Clock::now();
}

Analyzer::~Analyzer()
{
}

void Analyzer::BeginFrame()
{
    frameStart_ = Clock::now();
    frameStarted_ = true;
    sectionTimes_.clear();
}

void Analyzer::EndFrame()
{
    if (!frameStarted_)
        return;
    
    auto now = Clock::now();
    float frameTimeMs = std::chrono::duration<float, std::milli>(now - frameStart_).count();
    
    // Store frame time
    frameTimes_.push_back(frameTimeMs);
    if (frameTimes_.size() > maxSamples_)
        frameTimes_.pop_front();
    
    // Calculate FPS for this frame
    float currentFps = (frameTimeMs > 0.001f) ? (1000.0f / frameTimeMs) : 0.0f;
    fpsHistory_.push_back(currentFps);
    if (fpsHistory_.size() > maxSamples_)
        fpsHistory_.pop_front();
    
    // Update metrics
    CalculateMetrics();
    CountSceneObjects();
    
    lastFrameEnd_ = now;
    frameStarted_ = false;
}

void Analyzer::Update()
{
    // Legacy method - combines EndFrame and BeginFrame
    EndFrame();
    BeginFrame();
}

void Analyzer::Reset()
{
    frameTimes_.clear();
    fpsHistory_.clear();
    sectionTimes_.clear();
    metrics_ = PerformanceMetrics{};
}

float Analyzer::GetSectionTime(const std::string& name) const
{
    auto it = sectionTimes_.find(name);
    return (it != sectionTimes_.end()) ? it->second : 0.0f;
}

void Analyzer::RecordSectionTime(const std::string& name, float timeMs)
{
    sectionTimes_[name] = timeMs;
}

void Analyzer::CalculateMetrics()
{
    if (frameTimes_.empty())
        return;
    
    // Current frame time and FPS
    metrics_.frameTime = frameTimes_.back();
    metrics_.fps = fpsHistory_.back();
    
    // Average frame time
    float sumFrameTime = std::accumulate(frameTimes_.begin(), frameTimes_.end(), 0.0f);
    metrics_.avgFrameTime = sumFrameTime / static_cast<float>(frameTimes_.size());
    
    // Average FPS
    float sumFps = std::accumulate(fpsHistory_.begin(), fpsHistory_.end(), 0.0f);
    metrics_.avgFps = sumFps / static_cast<float>(fpsHistory_.size());
    
    // Min/Max FPS
    auto [minIt, maxIt] = std::minmax_element(fpsHistory_.begin(), fpsHistory_.end());
    metrics_.minFps = *minIt;
    metrics_.maxFps = *maxIt;
    
    // Frame time variance (jitter)
    float varianceSum = 0.0f;
    for (float ft : frameTimes_)
    {
        float diff = ft - metrics_.avgFrameTime;
        varianceSum += diff * diff;
    }
    metrics_.frameTimeVariance = varianceSum / static_cast<float>(frameTimes_.size());
    
    // 1% Low FPS - average of worst 1% of frames
    if (fpsHistory_.size() >= 10)
    {
        std::vector<float> sortedFps(fpsHistory_.begin(), fpsHistory_.end());
        std::sort(sortedFps.begin(), sortedFps.end());
        
        size_t onePercentCount = std::max(size_t(1), sortedFps.size() / 100);
        float sum1Percent = 0.0f;
        for (size_t i = 0; i < onePercentCount; ++i)
            sum1Percent += sortedFps[i];
        
        metrics_.percentile1Low = sum1Percent / static_cast<float>(onePercentCount);
    }
    else
    {
        metrics_.percentile1Low = metrics_.minFps;
    }
}

void Analyzer::CountSceneObjects()
{
    if (!ctx_.sceneProvider)
        return;
    
    Scene* scene = ctx_.sceneProvider->GetCurrentScene();
    if (!scene)
    {
        metrics_.gameObjectCount = 0;
        metrics_.componentCount = 0;
        metrics_.renderableCount = 0;
        metrics_.colliderCount = 0;
        return;
    }
    
    auto gameObjects = scene->GetGameObjects();
    metrics_.gameObjectCount = gameObjects.size();
    
    size_t componentCount = 0;
    size_t renderableCount = 0;
    size_t colliderCount = 0;
    
    for (GameObject* go : gameObjects)
    {
        if (!go || go->IsDestroyed())
            continue;
        
        const auto& components = go->GetAllComponents();
        componentCount += components.size();
        
        if (go->HasCollider())
            ++colliderCount;
        
        // Count renderables (simplified - assumes 1 per visible object)
        if (go->IsEnabled())
            ++renderableCount;
    }
    
    metrics_.componentCount = componentCount;
    metrics_.renderableCount = renderableCount;
    metrics_.colliderCount = colliderCount;
}

} // namespace Axion
