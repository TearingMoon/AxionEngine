#pragma once

#include "axion_engine/Axion.hpp"
#include <glm/glm.hpp>

namespace Axion
{

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
        auto *owner = GetOwner();
        if (!owner || owner->IsDestroyed()) return;
        
        auto *tr = owner->GetTransform();
        if (!tr) return;
        
        // Validate target is not null and not destroyed
        if (!target || target->IsDestroyed())
            return;
        
        auto *targetTr = target->GetTransform();
        if (!targetTr) return;

        float deltaTime = context.timeProvider->GetDeltaTime();
        
        glm::vec3 targetPos = targetTr->GetPosition();
        glm::vec3 currentPos = tr->GetPosition();
        
        // Lerp towards target
        glm::vec3 desiredPos = targetPos;
        desiredPos.z = currentPos.z; // Keep the same Z depth
        
        glm::vec3 smoothedPos = glm::mix(currentPos, desiredPos, smoothSpeed * deltaTime);
        tr->SetPosition(smoothedPos);
    }
};

} // namespace Axion
