#pragma once

#include "ColliderComponent.hpp"
#include <glm/glm.hpp>

class OBBColliderComponent : public ColliderComponent
{
public:
    OBBColliderComponent() = default;

    void SetSize(glm::vec3 size)
    {
        size_ = size;
    }

    glm::vec3 GetSize() const;

    // Collisions
    bool Intersects(const ColliderComponent &other, Manifold &out) const override;
    bool IntersectsWithCircle(const class CircleColliderComponent &circle, Manifold &out) const override;
    bool IntersectsWithAABB(const class AABBColliderComponent &aabb, Manifold &out) const override;
    bool IntersectsWithOBB(const class OBBColliderComponent &other, Manifold &out) const override;

    // Render
    void Render(const RenderContext &ctx) override;

private:
    glm::vec3 size_{1.0f, 1.0f, 1.0f};

    glm::vec2 GetHalfExtents2D() const;
    glm::vec2 GetCenter2D() const;
    void GetAxes2D(glm::vec2 &axisX, glm::vec2 &axisY) const;
};