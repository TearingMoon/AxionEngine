#pragma once

#include "axion_engine/Axion.hpp"

namespace Axion
{

class ReticleScript : public ScriptableComponent
{
public:
    void Start(EngineContext &context) override
    {
        // Initialize any necessary variables
    }

    void Update(EngineContext &context) override
    {
        auto *owner = GetOwner();
        if (!owner || owner->IsDestroyed()) return;
        
        auto *tr = owner->GetTransform();
        if (!tr) return;
        
        // Put the reticle at the mouse position
        auto input = context.input;
        auto window = context.window;

        // Get mouse position
        SDL_Point mousePos = input->GetMousePosition();
        int mouseX = mousePos.x;
        int mouseY = mousePos.y;

        // Get window size using SDL directly
        int winW = 0, winH = 0;
        SDL_GetWindowSize(window->GetSDLWindow(), &winW, &winH);
        
        // Get camera position to adjust world coordinates
        glm::vec3 cameraPos(0.0f);
        if (context.scene)
        {
            auto currentScene = context.scene->GetCurrentScene();
            if (currentScene)
            {
                auto camera = currentScene->GetCurrentCamera();
                if (camera)
                {
                    auto *camOwner = camera->GetOwner();
                    if (camOwner && !camOwner->IsDestroyed())
                    {
                        auto *cameraTr = camOwner->GetTransform();
                        if (cameraTr)
                        {
                            cameraPos = cameraTr->GetPosition();
                        }
                    }
                }
            }
        }

        float worldX = (mouseX - winW / 2.0f) + cameraPos.x;
        float worldY = (winH / 2.0f - mouseY) + cameraPos.y;

        // Update the reticle position
        tr->SetPosition({worldX, worldY, 0.0f});
    }
};

} // namespace Axion