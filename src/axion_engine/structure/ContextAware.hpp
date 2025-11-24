#pragma once
#include "axion_engine/core/EngineContext.hpp"

class ContextAware
{
public:
    explicit ContextAware(EngineContext &ctx) : ctx_(ctx) {}

protected:
    EngineContext &ctx_;
};