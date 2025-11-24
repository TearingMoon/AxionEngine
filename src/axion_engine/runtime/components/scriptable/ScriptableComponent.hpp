#pragma once

#include "axion_engine/runtime/components/Component.hpp"

#include "axion_engine/runtime/interfaces/IUpdateable.hpp"
#include "axion_engine/runtime/interfaces/ICollisionListener.hpp"

class ScriptableComponent : public Component, public IUpdateable, public ICollisionListener
{
public:
    virtual ~ScriptableComponent() = default;
};