#pragma once

#include "ColliderComponent.hpp"

class SphereColliderComponent : public ColliderComponent
{
public:
    SphereColliderComponent() = default;
    void SetRadius(float radius) { radius_ = radius; }
    float GetRadius() const { return radius_; }
private:
    float radius_ = 1.0f;
};