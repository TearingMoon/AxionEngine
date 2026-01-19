#pragma once
#include <glm/glm.hpp>
#include <algorithm>

namespace Axion
{

/**
 * @brief Collision manifold containing intersection data.
 */
struct Manifold
{
    bool hit = false;           ///< Whether a collision occurred
    glm::vec2 normal{0, 0};     ///< Collision normal (pointing from A to B)
    float penetration = 0;      ///< Penetration depth
};

inline float Clamp(float v, float lo, float hi)
{
    return std::max(lo, std::min(v, hi));
}

inline glm::vec2 ClosestPointOnAABB(glm::vec2 p, glm::vec2 minB, glm::vec2 maxB)
{
    return {
        Clamp(p.x, minB.x, maxB.x),
        Clamp(p.y, minB.y, maxB.y)};
}

inline float Dot(const glm::vec2& a, const glm::vec2& b) { return a.x * b.x + a.y * b.y; }

inline float Abs(float v) { return std::fabs(v); }

inline void ProjectOBB(const glm::vec2& center,
                       const glm::vec2& axisX,
                       const glm::vec2& axisY,
                       const glm::vec2& half,
                       const glm::vec2& axis,
                       float& outMin, float& outMax)
{
    const float c = Dot(center, axis);
    const float r =
        Abs(Dot(axisX, axis)) * half.x +
        Abs(Dot(axisY, axis)) * half.y;

    outMin = c - r;
    outMax = c + r;
}

inline bool OverlapIntervals(float aMin, float aMax, float bMin, float bMax, float& outOverlap)
{
    const float left = std::max(aMin, bMin);
    const float right = std::min(aMax, bMax);
    outOverlap = right - left;
    return outOverlap > 0.0f;
}

inline bool SAT_ObbObb(const glm::vec2& cA, const glm::vec2& axA, const glm::vec2& ayA, const glm::vec2& hA,
                       const glm::vec2& cB, const glm::vec2& axB, const glm::vec2& ayB, const glm::vec2& hB,
                       Manifold& out)
{
    const glm::vec2 axes[4] = {axA, ayA, axB, ayB};

    float minPen = 1e30f;
    glm::vec2 bestAxis{0, 0};

    const glm::vec2 d = cB - cA;

    for (const glm::vec2& axisRaw : axes)
    {
        glm::vec2 axis = axisRaw;
        const float len2 = Dot(axis, axis);
        if (len2 < 1e-8f)
            continue;
        axis *= 1.0f / std::sqrt(len2);

        float aMin, aMax, bMin, bMax;
        ProjectOBB(cA, axA, ayA, hA, axis, aMin, aMax);
        ProjectOBB(cB, axB, ayB, hB, axis, bMin, bMax);

        float overlap;
        if (!OverlapIntervals(aMin, aMax, bMin, bMax, overlap))
            return false;

        if (overlap < minPen)
        {
            minPen = overlap;
            bestAxis = (Dot(d, axis) >= 0.0f) ? axis : -axis;
        }
    }

    out.hit = true;
    out.penetration = minPen;
    out.normal = bestAxis;
    return true;
}

inline float DegToRad(float deg)
{
    return deg * (3.14159265358979323846f / 180.0f);
}

} // namespace Axion