#pragma once
#include "axion_engine/core/EngineContext.hpp"

class GameObject;

class Component
{
public:
    virtual ~Component() = default;

    void Bind(GameObject *owner) { owner_ = owner; }
    GameObject *GetOwner() const { return owner_; }

    virtual void OnMounted(EngineContext &context) {};
    virtual void OnDestroy(EngineContext &context) {};

    virtual void OnEnabled(EngineContext &context) {};
    virtual void OnDisabled(EngineContext &context) {};

private:
    GameObject *owner_ = nullptr;
};