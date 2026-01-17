#pragma once

#include "axion_engine/runtime/components/Component.hpp"
#include "axion_engine/runtime/interfaces/IRenderable.hpp"
#include "ColliderHelpers.hpp"

enum COLLIDER_TYPE
{
    TRIGGER,
    PHYSICS
};

enum COLLIDER_SHAPE
{
    UNDEFINED,
    CIRCLE,
    AABB,
    OBB
};

class ColliderComponent : public Component, public IRenderable
{
public:
    ColliderComponent() = default;
    bool IsTrigger() const { return type_ == COLLIDER_TYPE::TRIGGER; }
    void SetColliderType(COLLIDER_TYPE type) { type_ = type; }

    GameObject *GetGameObject() const { return GetOwner(); }
    COLLIDER_SHAPE GetShape() const { return shape_; }

    // Collision
    virtual bool Intersects(const ColliderComponent &other, Manifold &out) const = 0;
    virtual bool IntersectsWithCircle(const class CircleColliderComponent &, Manifold &) const = 0;
    virtual bool IntersectsWithAABB(const class AABBColliderComponent &, Manifold &) const = 0;
    virtual bool IntersectsWithOBB(const class OBBColliderComponent &, Manifold &) const = 0;

    // IRenderable implementation
    RenderLayer GetLayer() const override { return RenderLayer::Debug; }
    int GetOrderInLayer() const override { return 0; }
    int GetSortKey() const override { return 0; }

private:
    COLLIDER_TYPE type_ = COLLIDER_TYPE::TRIGGER;
    COLLIDER_SHAPE shape_ = COLLIDER_SHAPE::UNDEFINED;
};