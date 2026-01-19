#pragma once

#include "axion_engine/runtime/components/Component.hpp"
#include "axion_engine/runtime/interfaces/IRenderable.hpp"
#include "ColliderHelpers.hpp"

namespace Axion
{

/**
 * @brief Type of collider behavior.
 */
enum class ColliderType
{
    Trigger,  ///< Detects overlaps but doesn't resolve physics
    Physics   ///< Participates in physical collision resolution
};

/**
 * @brief Shape type for collision detection.
 */
enum class ColliderShape
{
    Undefined,  ///< Not yet specified
    Circle,     ///< Circular collider
    AABB,       ///< Axis-Aligned Bounding Box
    OBB         ///< Oriented Bounding Box
};

class CircleColliderComponent;
class AABBColliderComponent;
class OBBColliderComponent;

/**
 * @brief Abstract base class for all collider components.
 *
 * ColliderComponent provides the common interface for collision detection
 * using double dispatch pattern. Supports trigger and physics modes.
 *
 * All colliders are also renderable for debug visualization purposes.
 */
class ColliderComponent : public Component, public IRenderable
{
public:
    ColliderComponent() = default;
    virtual ~ColliderComponent() = default;

    /**
     * @brief Check if this collider is a trigger (non-physical).
     * @return True if trigger mode, false if physics mode.
     */
    bool IsTrigger() const { return type_ == ColliderType::Trigger; }

    /**
     * @brief Set the collider behavior type.
     * @param type Trigger or Physics mode.
     */
    void SetColliderType(ColliderType type) { type_ = type; }

    /**
     * @brief Get the owning GameObject.
     * @return Pointer to the owner.
     */
    GameObject* GetGameObject() const { return GetOwner(); }

    /**
     * @brief Get the shape type of this collider.
     * @return The collider shape.
     */
    ColliderShape GetShape() const { return shape_; }

    /**
     * @brief Test intersection with another collider.
     * @param other The other collider.
     * @param out Output collision manifold.
     * @return True if intersecting.
     */
    virtual bool Intersects(const ColliderComponent& other, Manifold& out) const = 0;

    /**
     * @brief Test intersection with a circle collider.
     * @param circle The circle collider.
     * @param out Output collision manifold.
     * @return True if intersecting.
     */
    virtual bool IntersectsWithCircle(const CircleColliderComponent& circle, Manifold& out) const = 0;

    /**
     * @brief Test intersection with an AABB collider.
     * @param aabb The AABB collider.
     * @param out Output collision manifold.
     * @return True if intersecting.
     */
    virtual bool IntersectsWithAABB(const AABBColliderComponent& aabb, Manifold& out) const = 0;

    /**
     * @brief Test intersection with an OBB collider.
     * @param obb The OBB collider.
     * @param out Output collision manifold.
     * @return True if intersecting.
     */
    virtual bool IntersectsWithOBB(const OBBColliderComponent& obb, Manifold& out) const = 0;

    // IRenderable implementation
    RenderLayer GetLayer() const override { return RenderLayer::Debug; }
    int GetOrderInLayer() const override { return 0; }

protected:
    ColliderType type_ = ColliderType::Trigger;
    ColliderShape shape_ = ColliderShape::Undefined;
};

} // namespace Axion