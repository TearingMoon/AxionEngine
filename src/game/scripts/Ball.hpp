#pragma once

#include "axion_engine/Axion.hpp"
#include <functional>

namespace Axion
{

class Ball : public ScriptableComponent
{
public:
    void OnMounted(EngineContext& context) override
    {
        transform = GetOwner()->GetTransform();
        ResetBall();
    }

    void Update(EngineContext& context) override
    {
        if (!transform) return;

        glm::vec3 pos = transform->GetPosition();
        pos.x += velocity.x * context.time->GetDeltaTime();
        pos.y += velocity.y * context.time->GetDeltaTime();

        if (pos.y > maxY || pos.y < minY)
        {
            velocity.y = -velocity.y;
            pos.y = glm::clamp(pos.y, minY, maxY);
        }

        if (pos.x > maxX)
        {
            if (onPlayer1Score) onPlayer1Score();
            ResetBall();
            return;
        }
        else if (pos.x < minX)
        {
            if (onPlayer2Score) onPlayer2Score();
            ResetBall();
            return;
        }

        transform->SetPosition(pos);
    }

    void OnCollisionEnter(GameObject& other) override
    {
        velocity.x = -velocity.x;

        if (transform && other.GetTransform())
        {
            float ballY = transform->GetPosition().y;
            float paddleY = other.GetTransform()->GetPosition().y;
            float diff = ballY - paddleY;
            velocity.y += diff * 2.0f;
        }

        velocity *= 1.05f;

        if (std::abs(velocity.x) > maxSpeed) velocity.x = (velocity.x > 0) ? maxSpeed : -maxSpeed;
        if (std::abs(velocity.y) > maxSpeed) velocity.y = (velocity.y > 0) ? maxSpeed : -maxSpeed;
    }

    void ResetBall()
    {
        if (!transform) return;

        transform->SetPosition({0.0f, 0.0f, 0.0f});

        float dirX = (rand() % 2 == 0) ? 1.0f : -1.0f;
        float dirY = ((rand() % 100) / 100.0f - 0.5f) * 2.0f;

        velocity = glm::vec2(dirX * initialSpeed, dirY * initialSpeed * 0.5f);
    }

    void SetBounds(float minX_, float maxX_, float minY_, float maxY_)
    {
        minX = minX_;
        maxX = maxX_;
        minY = minY_;
        maxY = maxY_;

        float screenWidth = maxX - minX;
        maxSpeed = screenWidth * 0.75f;
    }

    void SetInitialSpeed(float speed)
    {
        initialSpeed = speed;
    }

    void StopBall()
    {
        velocity = glm::vec2(0.0f, 0.0f);
    }

    void SetOnPlayer1Score(std::function<void()> callback) { onPlayer1Score = callback; }
    void SetOnPlayer2Score(std::function<void()> callback) { onPlayer2Score = callback; }

private:
    TransformComponent* transform = nullptr;
    glm::vec2 velocity = {200.0f, 0.0f};
    float initialSpeed = 250.0f;
    float maxSpeed = 600.0f;
    float minX = -400.0f;
    float maxX = 400.0f;
    float minY = -300.0f;
    float maxY = 300.0f;

    std::function<void()> onPlayer1Score;
    std::function<void()> onPlayer2Score;
};

} // namespace Axion
