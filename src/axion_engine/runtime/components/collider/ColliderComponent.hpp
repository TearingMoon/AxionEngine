#pragma once

#include "axion_engine/runtime/components/Component.hpp"
#include "axion_engine/runtime/interfaces/IRenderable.hpp"

enum COLLIDER_TYPE
{
    TRIGGER,
    PHYSICS
};

class ColliderComponent : public Component, public IRenderable
{
public:
    ColliderComponent() = default;
    bool IsTrigger() const { return type_ == COLLIDER_TYPE::TRIGGER; }
    void SetColliderType(COLLIDER_TYPE type) { type_ = type; }

    GameObject *GetGameObject() const { return GetOwner(); }

    RenderLayer GetLayer() const override { return RenderLayer::Debug; }
    int GetOrderInLayer() const override { return 0; }
    int GetSortKey() const override { return 0; }

private:
    COLLIDER_TYPE type_ = COLLIDER_TYPE::TRIGGER;
};