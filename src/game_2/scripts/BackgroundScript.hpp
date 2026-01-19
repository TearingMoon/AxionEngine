#pragma once

#include "axion_engine/runtime/components/scriptable/ScriptableComponent.hpp"
#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include "axion_engine/runtime/components/camera/CameraComponent.hpp"
#include "axion_engine/managers/scene/SceneManager.hpp"
#include <glm/glm.hpp>

class BackgroundScript : public ScriptableComponent
{
public:
    void Update(EngineContext &context) override
    {
        auto *owner = GetOwner();
        if (!owner || owner->IsDestroyed()) return;
        
        auto *tr = owner->GetTransform();
        if (!tr) return;
        
        // Get current camera from scene
        auto *scene = context.scene->GetCurrentScene();
        if (!scene) return;
        
        auto *camera = scene->GetCurrentCamera();
        if (!camera) return;
        
        auto *camOwner = camera->GetOwner();
        if (!camOwner || camOwner->IsDestroyed()) return;
        
        auto *camTr = camOwner->GetTransform();
        if (!camTr) return;
        
        // Follow camera position (keep Z depth)
        glm::vec3 camPos = camTr->GetPosition();
        glm::vec3 currentPos = tr->GetPosition();
        tr->SetPosition({camPos.x, camPos.y, currentPos.z});
    }
};
