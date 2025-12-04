#pragma once

#define AXION_CTIME ctx_.time
#define AXION_CINPUT ctx_.input
#define AXION_CSCENE ctx_.scene
#define AXION_CPHYSICS ctx_.physics
#define AXION_CRENDER ctx_.render
#define AXION_CASSETS ctx_.assets
#define AXION_CWINDOW ctx_.window

#include "axion_engine/core/EngineContext.hpp"

class ContextAware
{
public:
    explicit ContextAware(EngineContext &ctx) : ctx_(ctx) {}

protected:
    EngineContext &ctx_;
};