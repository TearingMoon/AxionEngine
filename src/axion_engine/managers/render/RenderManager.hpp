#pragma once
#include <cstdio>

#include "axion_engine/structure/ContextAware.hpp"
#include "axion_engine/core/EngineContext.hpp"

class RenderManager : public ContextAware
{
    public:
    RenderManager(EngineContext &ctx);
    ~RenderManager();

    void Update();
};