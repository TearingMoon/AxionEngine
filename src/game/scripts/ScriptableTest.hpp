#pragma once

#include "axion_engine/runtime/components/scriptable/ScriptableComponent.hpp"

class ScriptableTest : public ScriptableComponent
{
public:
    ScriptableTest() {}

    void OnMounted(EngineContext &context) override
    {
        printf("- ScriptableTest Mounted!\n");
    }

    // TODO: Review if this is called correctly
    void OnEnabled(EngineContext &context) override
    {
        printf("- ScriptableTest Enabled!\n");
    }

    bool updateFlag = true;
    void Update(EngineContext &context) override
    {
        if (updateFlag)
        {
            printf("- ScriptableTest Updated!\n");
            updateFlag = false;
        }
    }

    bool fixedUpdateFlag = true;
    void FixedUpdate(EngineContext &context) override
    {
        if (fixedUpdateFlag)
        {
            printf("- ScriptableTest Fixed Updated!\n");
            fixedUpdateFlag = false;
        }
    }

    void Start(EngineContext &context) override
    {
        printf("- ScriptableTest Started!\n");
    }

    void OnDisabled(EngineContext &context) override
    {
        printf("- ScriptableTest Disabled!\n");
    }

    void OnDestroy(EngineContext &context) override
    {
        printf("- ScriptableTest Destroyed!\n");
    }
};