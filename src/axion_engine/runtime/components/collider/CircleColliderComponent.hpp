#pragma once

#include "ColliderComponent.hpp"

class CircleColliderComponent : public ColliderComponent
{
public:
    CircleColliderComponent() = default;
    void SetRadius(float radius) { radius_ = radius; }
    float GetRadius() const;

    // ColliderComponent pure virtual implementations
    bool Intersects(const ColliderComponent &other, Manifold &out) const override;
    bool IntersectsWithCircle(const class CircleColliderComponent &other, Manifold &out) const override;
    bool IntersectsWithAABB(const class AABBColliderComponent &other, Manifold &out) const override;
    bool IntersectsWithOBB(const class OBBColliderComponent &other, Manifold &out) const override;

    void Render(const RenderContext &ctx) override;

private:
    float radius_ = 1.0f;
    void DrawCircle(const RenderContext &ctx, int centerX, int centerY, int radius);
};