#pragma once

#include "axion_engine/runtime/components/scriptable/ScriptableComponent.hpp"
#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include <glm/glm.hpp>

class CameraFollowScript : public ScriptableComponent
{
public:
    GameObject *target = nullptr;
    float smoothSpeed = 5.0f;

    void Start(EngineContext &context) override
    {
    }

    void Update(EngineContext &context) override
    {
        // Validate target is not null and not destroyed
        if (!target || target->IsDestroyed())
            return;

        float deltaTime = context.time->GetDeltaTime();
        auto tr = GetOwner()->GetTransform();
        
        glm::vec3 targetPos = target->GetTransform()->GetPosition();
        glm::vec3 currentPos = tr->GetPosition();
        
        // Lerp towards target
        glm::vec3 desiredPos = targetPos;
        desiredPos.z = currentPos.z; // Keep the same Z depth
        
        glm::vec3 smoothedPos = glm::mix(currentPos, desiredPos, smoothSpeed * deltaTime);
        tr->SetPosition(smoothedPos);
    }
};
