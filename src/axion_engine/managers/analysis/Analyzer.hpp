#pragma once
#include "axion_engine/structure/ContextAware.hpp"

class Analyzer : public ContextAware
{
public:
    Analyzer(EngineContext &ctx);
    ~Analyzer();

    void Update();
};
