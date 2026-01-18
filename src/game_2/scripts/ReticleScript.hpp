#pragma once

#include "axion_engine/runtime/components/scriptable/ScriptableComponent.hpp"
#include "axion_engine/platform/window/Window.hpp"
#include "axion_engine/managers/input/InputManager.hpp"
#include "axion_engine/managers/scene/SceneManager.hpp"

class ReticleScript : public ScriptableComponent
{
public:
    void Start(EngineContext &context) override
    {
        // Initialize any necessary variables
    }

    void Update(EngineContext &context) override
    {
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
                    auto cameraTr = camera->GetOwner()->GetTransform();
                    if (cameraTr)
                    {
                        cameraPos = cameraTr->GetPosition();
                    }
                }
            }
        }

        float worldX = (mouseX - winW / 2.0f) + cameraPos.x;
        float worldY = (winH / 2.0f - mouseY) + cameraPos.y;

        // Update the reticle position
        auto tr = GetOwner()->GetTransform();
        tr->SetPosition({worldX, worldY, 0.0f});
    }
};