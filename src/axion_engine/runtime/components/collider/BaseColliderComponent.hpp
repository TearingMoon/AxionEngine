#pragma once
#include "axion_math/Vector.hpp"
#include "axion_engine/runtime/components/Component.hpp"

enum COLLIDER_TYPE {
    TRIGGER,
    PHYSICS
};

class BaseColliderComponent: public Component
{
public:
    BaseColliderComponent() = default;
private:
    COLLIDER_TYPE type_ = COLLIDER_TYPE::TRIGGER;
};