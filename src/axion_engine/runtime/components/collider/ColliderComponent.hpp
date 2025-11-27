#pragma once
#include "axion_engine/runtime/components/Component.hpp"

enum COLLIDER_TYPE
{
    TRIGGER,
    PHYSICS
};

class ColliderComponent : public Component
{
public:
    ColliderComponent() = default;
    bool IsTrigger() const { return type_ == COLLIDER_TYPE::TRIGGER; }
    void SetColliderType(COLLIDER_TYPE type) { type_ = type; }

private:
    COLLIDER_TYPE type_ = COLLIDER_TYPE::TRIGGER;
};