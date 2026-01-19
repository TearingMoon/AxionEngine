#pragma once

#include "ColliderComponent.hpp"

namespace Axion
{

/**
 * @brief Circular collider component.
 *
 * Uses a radius for collision detection. Rotation-invariant
 * and efficient for circular or spherical objects.
 */
class CircleColliderComponent : public ColliderComponent
{
public:
    CircleColliderComponent();
    ~CircleColliderComponent() override = default;

    /**
     * @brief Set the collision radius.
     * @param radius The radius value.
     */
    void SetRadius(float radius) { radius_ = radius; }

    /**
     * @brief Get the current radius (scaled by transform).
     * @return The effective collision radius.
     */
    float GetRadius() const;

    // Collision interface
    bool Intersects(const ColliderComponent& other, Manifold& out) const override;
    bool IntersectsWithCircle(const CircleColliderComponent& other, Manifold& out) const override;
    bool IntersectsWithAABB(const AABBColliderComponent& other, Manifold& out) const override;
    bool IntersectsWithOBB(const OBBColliderComponent& other, Manifold& out) const override;

    void Render(const RenderContext& ctx) override;

private:
    float radius_ = 1.0f;

    void DrawCircle(const RenderContext& ctx, int centerX, int centerY, int radius);
};

} // namespace Axion