#pragma once

#include "axion_engine/runtime/components/scriptable/ScriptableComponent.hpp"
#include "axion_engine/runtime/components/transform/TransformComponent.hpp"
#include "axion_engine/managers/time/TimeManager.hpp"
#include "axion_engine/managers/input/InputManager.hpp"
#include "axion_engine/managers/input/InputManager.hpp"

class PlayerPaddle : public ScriptableComponent
{
public:
    void OnMounted(EngineContext &context) override
    {
        transform = GetOwner()->GetTransform();
    }

    void Update(EngineContext &context) override
    {
        if (!transform)
            return;

        glm::vec3 pos = transform->GetPosition();

        if (context.input->IsKeyDown(SDL_SCANCODE_W))
        {
            pos.y -= speed * context.time->GetDeltaTime();
        }
        if (context.input->IsKeyDown(SDL_SCANCODE_S))
        {
            pos.y += speed * context.time->GetDeltaTime();
        }

        if (pos.y > maxY)
            pos.y = maxY;
        if (pos.y < minY)
            pos.y = minY;

        transform->SetPosition(pos);
    }

    void SetBounds(float min, float max)
    {
        minY = min;
        maxY = max;
    }

    void SetSpeed(float s) { speed = s; }

private:
    TransformComponent *transform = nullptr;
    float speed = 300.0f;
    float minY = -250.0f;
    float maxY = 250.0f;
};
