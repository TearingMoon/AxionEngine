#pragma once
#include "axion_engine/runtime/components/Component.hpp"

enum COLLIDER_TYPE
{
    TRIGGER,
    PHYSICS
};

class BaseColliderComponent : public Component
{
public:
    BaseColliderComponent() = default;
    bool IsTrigger() const { return type_ == COLLIDER_TYPE::TRIGGER; }
    void SetColliderType(COLLIDER_TYPE type) { type_ = type; }

private:
    COLLIDER_TYPE type_ = COLLIDER_TYPE::TRIGGER;
};