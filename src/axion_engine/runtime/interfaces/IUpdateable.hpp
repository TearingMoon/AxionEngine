#pragma once
#include "axion_engine/core/EngineContext.hpp"

class IUpdateable
{
public:
    virtual ~IUpdateable() = default;
    
    virtual void Start(EngineContext &context) {};
    virtual void Update(EngineContext &context) {};

    virtual void FixedUpdate(EngineContext &context) {};
    virtual void LateUpdate(EngineContext &context) {};
};