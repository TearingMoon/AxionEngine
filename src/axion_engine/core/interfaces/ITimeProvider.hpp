#pragma once

namespace Axion
{

/**
 * @brief Interface for time-related queries.
 * 
 * Provides read-only access to timing information without
 * exposing the full TimeManager implementation.
 */
class ITimeProvider
{
public:
    virtual ~ITimeProvider() = default;
    
    /** @brief Returns the time elapsed since the last frame in seconds. */
    virtual float GetDeltaTime() const = 0;
    
    /** @brief Returns the total elapsed time since engine start in seconds. */
    virtual float GetElapsedTime() const = 0;
    
    /** @brief Returns the current frames per second. */
    virtual float GetFPS() const = 0;
};

} // namespace Axion
