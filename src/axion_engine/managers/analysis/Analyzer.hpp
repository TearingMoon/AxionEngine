#pragma once
#include <chrono>
#include <deque>
#include <string>
#include <unordered_map>

#include "axion_engine/structure/ContextAware.hpp"

namespace Axion
{

/**
 * @brief Performance metrics snapshot.
 */
struct PerformanceMetrics
{
    // Frame timing
    float fps = 0.0f;               ///< Current frames per second
    float avgFps = 0.0f;            ///< Average FPS over sample window
    float minFps = 0.0f;            ///< Minimum FPS in sample window
    float maxFps = 0.0f;            ///< Maximum FPS in sample window
    float frameTime = 0.0f;         ///< Current frame time in ms
    float avgFrameTime = 0.0f;      ///< Average frame time in ms
    
    // Frame stability
    float frameTimeVariance = 0.0f; ///< Frame time variance (jitter)
    float percentile1Low = 0.0f;    ///< 1% low FPS (worst frames)
    
    // Memory (basic)
    size_t gameObjectCount = 0;     ///< Active GameObjects
    size_t componentCount = 0;      ///< Active Components
    
    // Scene
    size_t renderableCount = 0;     ///< Objects being rendered
    size_t colliderCount = 0;       ///< Active colliders
};

/**
 * @brief Scoped timer for profiling code sections.
 * 
 * @par Usage:
 * @code
 * {
 *     ScopedTimer timer(ctx_.analyzer, "Physics");
 *     // Physics code here...
 * } // Timer stops automatically
 * @endcode
 */
class ScopedTimer
{
public:
    ScopedTimer(class Analyzer* analyzer, const std::string& name);
    ~ScopedTimer();
    
private:
    class Analyzer* analyzer_;
    std::string name_;
    std::chrono::high_resolution_clock::time_point start_;
};

/**
 * @brief Performance analysis and profiling manager.
 * 
 * Analyzer provides comprehensive performance monitoring including:
 * - FPS tracking (current, average, min, max, 1% low)
 * - Frame time analysis and jitter detection
 * - GameObject/Component counting
 * - Section profiling with ScopedTimer
 * 
 * @par Usage:
 * @code
 * auto& metrics = ctx_.analyzer->GetMetrics();
 * std::cout << "FPS: " << metrics.fps << "\n";
 * std::cout << "1% Low: " << metrics.percentile1Low << "\n";
 * @endcode
 */
class Analyzer : public ContextAware
{
public:
    Analyzer(EngineContext& ctx);
    ~Analyzer();

    /** @brief Marks the start of a new frame. Call at frame begin. */
    void BeginFrame();
    
    /** @brief Marks the end of a frame and calculates metrics. */
    void EndFrame();
    
    /** @brief Updates analysis metrics (legacy, calls EndFrame + BeginFrame). */
    void Update();
    
    /** @brief Returns the current performance metrics. */
    const PerformanceMetrics& GetMetrics() const { return metrics_; }
    
    /** @brief Returns timing for a profiled section in ms. */
    float GetSectionTime(const std::string& name) const;
    
    /** @brief Returns all profiled section times. */
    const std::unordered_map<std::string, float>& GetSectionTimes() const { return sectionTimes_; }
    
    /** @brief Resets all accumulated statistics. */
    void Reset();
    
    /** @brief Sets the sample window size for averaging. */
    void SetSampleSize(size_t samples) { maxSamples_ = samples; }
    
    /** @brief Returns true if FPS is below threshold. */
    bool IsPerformanceWarning(float threshold = 30.0f) const { return metrics_.fps < threshold; }

private:
    friend class ScopedTimer;
    
    void RecordSectionTime(const std::string& name, float timeMs);
    void CalculateMetrics();
    void CountSceneObjects();
    
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::high_resolution_clock::time_point;
    
    TimePoint frameStart_;
    TimePoint lastFrameEnd_;
    bool frameStarted_ = false;
    
    std::deque<float> frameTimes_;      // Frame times in ms
    std::deque<float> fpsHistory_;       // FPS history
    size_t maxSamples_ = 120;            // ~2 seconds at 60fps
    
    std::unordered_map<std::string, float> sectionTimes_;
    
    PerformanceMetrics metrics_;
};

} // namespace Axion
