#pragma once

#include "ColliderComponent.hpp"
#include <glm/glm.hpp>

namespace Axion
{

/**
 * @brief Axis-Aligned Bounding Box collider component.
 *
 * AABB colliders are rectangular boxes aligned to world axes.
 * Fast for collision detection but cannot rotate.
 */
class AABBColliderComponent : public ColliderComponent
{
public:
    AABBColliderComponent();
    ~AABBColliderComponent() override = default;

    /**
     * @brief Set the size of the bounding box.
     * @param size Width, height, depth dimensions.
     */
    void SetSize(glm::vec3 size) { size_ = size; }

    /**
     * @brief Get the current size.
     * @return The bounding box dimensions.
     */
    glm::vec3 GetSize() const;

    // Collision interface
    bool Intersects(const ColliderComponent& other, Manifold& out) const override;
    bool IntersectsWithCircle(const CircleColliderComponent& circle, Manifold& out) const override;
    bool IntersectsWithAABB(const AABBColliderComponent& other, Manifold& out) const override;
    bool IntersectsWithOBB(const OBBColliderComponent& other, Manifold& out) const override;

    void Render(const RenderContext& ctx) override;

private:
    glm::vec3 size_{1.0f, 1.0f, 1.0f};

    void GetMinMax2D(glm::vec2& outMin, glm::vec2& outMax) const;
};

} // namespace Axion