#pragma once

#include "axion_engine/runtime/components/Component.hpp"
#include "axion_engine/runtime/interfaces/IUpdateable.hpp"
#include "axion_data/Transform.hpp"

class TransformComponent : public Component
{
public:
    TransformComponent() = default;
    TransformComponent(const Transform3D &initialTransform) : transform_(initialTransform) {}
private:
    Transform3D transform_ = {};
};