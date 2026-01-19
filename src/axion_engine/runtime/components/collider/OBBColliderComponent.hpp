#pragma once

#include "ColliderComponent.hpp"
#include <glm/glm.hpp>

namespace Axion
{

/**
 * @brief Oriented Bounding Box collider component.
 *
 * OBB colliders are rectangular boxes that rotate with the transform.
 * More expensive than AABB but supports rotated objects.
 */
class OBBColliderComponent : public ColliderComponent
{
public:
    OBBColliderComponent();
    ~OBBColliderComponent() override = default;

    /**
     * @brief Set the size of the bounding box.
     * @param size Width, height, depth dimensions.
     */
    void SetSize(glm::vec3 size) { size_ = size; }

    /**
     * @brief Get the current size (scaled by transform).
     * @return The bounding box dimensions.
     */
    glm::vec3 GetSize() const;

    // Collision interface
    bool Intersects(const ColliderComponent& other, Manifold& out) const override;
    bool IntersectsWithCircle(const CircleColliderComponent& circle, Manifold& out) const override;
    bool IntersectsWithAABB(const AABBColliderComponent& aabb, Manifold& out) const override;
    bool IntersectsWithOBB(const OBBColliderComponent& other, Manifold& out) const override;

    void Render(const RenderContext& ctx) override;

private:
    glm::vec3 size_{1.0f, 1.0f, 1.0f};

    glm::vec2 GetHalfExtents2D() const;
    glm::vec2 GetCenter2D() const;
    void GetAxes2D(glm::vec2& axisX, glm::vec2& axisY) const;
};

} // namespace Axion