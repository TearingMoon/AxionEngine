#pragma once

#include "axion_engine/runtime/components/scriptable/ScriptableComponent.hpp"

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