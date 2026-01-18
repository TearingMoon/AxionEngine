#pragma once

#include "axion_engine/runtime/components/scriptable/ScriptableComponent.hpp"

class ScriptableRotationTest : public ScriptableComponent
{
public:
    ScriptableRotationTest() {}

    void OnMounted(EngineContext &context) override
    {
    }

    bool fixedUpdateFlag = true;
    void FixedUpdate(EngineContext &context) override
    {
        this->GetOwner()->GetTransform()->Rotate(0.0f, 0.0f, 1.0f);
    }
};