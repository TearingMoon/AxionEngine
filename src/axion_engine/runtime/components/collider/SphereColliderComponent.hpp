#pragma once

#include "BaseColliderComponent.hpp"

class SphereColliderComponent : public BaseColliderComponent
{
public:
    SphereColliderComponent() = default;
    void SetRadius(float radius) { radius_ = radius; }
    float GetRadius() const { return radius_; }
private:
    float radius_ = 1.0f;
};