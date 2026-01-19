#pragma once

#include "axion_engine/Axion.hpp"

namespace Axion
{

class ScriptableDeleteTest : public ScriptableComponent
{
public:
    ScriptableDeleteTest() {}

    void OnDisabled(EngineContext &context) override
    {
        printf("- ScriptableDeleteTest Disabled!\n");
    }

    void OnDestroy(EngineContext &context) override
    {
        printf("- ScriptableDeleteTest Destroyed!\n");
    }
};

} // namespace Axion