#pragma once

#include "ColliderComponent.hpp"
#include <glm/glm.hpp>

class AABBColliderComponent : public ColliderComponent
{
public:
    AABBColliderComponent() = default;

    void SetSize(glm::vec3 size)
    {
        size_ = size;
    }

    glm::vec3 GetSize() const;

    // Collisions
    bool Intersects(const ColliderComponent &other, Manifold &out) const override;
    bool IntersectsWithCircle(const CircleColliderComponent &circle, Manifold &out) const override;
    bool IntersectsWithAABB(const AABBColliderComponent &other, Manifold &out) const override;
    bool IntersectsWithOBB(const class OBBColliderComponent &other, Manifold &out) const override;

    // Render
    void Render(const RenderContext &ctx) override;

private:
    glm::vec3 size_{1.0f, 1.0f, 1.0f};

    void GetMinMax2D(glm::vec2 &outMin, glm::vec2 &outMax) const;
};