#pragma once

#include "axion_engine/runtime/components/scriptable/ScriptableComponent.hpp"
#include "axion_engine/runtime/components/renderer/SpriteRenderComponent.hpp"
#include "axion_engine/managers/input/InputManager.hpp"

class ScriptableTest : public ScriptableComponent
{
public:
    ScriptableTest() {}

    SpriteRenderComponent *spriteRenderer = nullptr;
    void OnMounted(EngineContext &context) override
    {
        printf("- ScriptableTest Mounted!\n");

        spriteRenderer = GetOwner()->GetComponent<SpriteRenderComponent>();
    }

    void OnEnabled(EngineContext &context) override
    {
        printf("- ScriptableTest Enabled!\n");
    }

    bool updateFlag = true;

    bool deltaTimeSaved = false;
    std::vector<float> DeltaTimes;
    void Update(EngineContext &context) override
    {

        // Save the first 100 DeltaTime updates
        if (DeltaTimes.size() < 100)
        {
            DeltaTimes.push_back(
                context.time->GetDeltaTime());
        }
        else if (!deltaTimeSaved)
        {
            printf("Saved 100 DeltaTime updates.\n");
            // Calculate average delta time
            float sum = 0.0f;
            for (float dt : DeltaTimes)
            {
                sum += dt;
            }
            float average = sum / DeltaTimes.size();
            printf("- Average Update Time: %.6f seconds.\n", average);
            printf("- Average Update FPS: %.2f\n", 1.0f / average);
            deltaTimeSaved = true;
        }

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

        if (context.input->IsKeyDown(SDL_SCANCODE_A)){ //TODO: Simplify All calls to Tranforms
            //Rotate the entity by 1 degree on the Z axis
            GetOwner()->GetTransform()->Rotate(0.0f, 0.0f, 1.0f);
        }

        if (context.input->IsKeyDown(SDL_SCANCODE_D)){ 
            //Rotate the entity by -1 degree on the Z axis
            GetOwner()->GetTransform()->Rotate(0.0f, 0.0f, -1.0f);
        }

        if (context.input->IsKeyDown(SDL_SCANCODE_W)){
            //Move the entity up by 0.1 units
            GetOwner()->GetTransform()->SetPosition(GetOwner()->GetTransform()->GetPosition() + (GetOwner()->GetTransform()->GetUpVector() * 1.0f));
        }

        if (context.input->IsKeyDown(SDL_SCANCODE_S)){
            //Move the entity down by 0.1 units
            GetOwner()->GetTransform()->SetPosition(GetOwner()->GetTransform()->GetPosition() - (GetOwner()->GetTransform()->GetUpVector() * 1.0f));
        }

        if (context.input->IsKeyDown(SDL_SCANCODE_KP_PLUS)){ 
            spriteRenderer->SetSize(spriteRenderer->GetSize() + glm::vec2(1.0f, 1.0f));
        }

        if (context.input->IsKeyDown(SDL_SCANCODE_KP_MINUS)){
            spriteRenderer->SetSize(spriteRenderer->GetSize() - glm::vec2(1.0f, 1.0f));
        }
    }

    void Start(EngineContext &context) override
    {
        printf("- ScriptableTest Started!\n");
    }
};