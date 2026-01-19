#pragma once

#include "axion_engine/Axion.hpp"

namespace Axion
{

class AIPaddle : public ScriptableComponent
{
public:
    void OnMounted(EngineContext& context) override
    {
        transform = GetOwner()->GetTransform();
    }

    void Update(EngineContext& context) override
    {
        if (!transform || !ballTransform) return;

        glm::vec3 paddlePos = transform->GetPosition();
        glm::vec3 ballPos = ballTransform->GetPosition();

        float targetY = ballPos.y;
        float diff = targetY - paddlePos.y;

        if (std::abs(diff) > 10.0f)
        {
            float moveDir = (diff > 0) ? 1.0f : -1.0f;
            paddlePos.y += moveDir * speed * context.time->GetDeltaTime();
        }

        if (paddlePos.y > maxY) paddlePos.y = maxY;
        if (paddlePos.y < minY) paddlePos.y = minY;

        transform->SetPosition(paddlePos);
    }

    void SetBallTransform(TransformComponent* ball) { ballTransform = ball; }
    void SetBounds(float min, float max)
    {
        minY = min;
        maxY = max;
    }
    void SetSpeed(float s) { speed = s; }

private:
    TransformComponent* transform = nullptr;
    TransformComponent* ballTransform = nullptr;
    float speed = 250.0f;
    float minY = -250.0f;
    float maxY = 250.0f;
};

} // namespace Axion
