#pragma once
#include "RenderComponent.hpp"
#include "axion_engine/runtime/components/transform/TransformComponent.hpp"

class CircleRenderComponent : public RenderComponent
{
public:
    void Render(const RenderContext &ctx) override
    {
        // Implementation for rendering a circle
        if (!ctx.renderer)
            return;

        auto *tr = GetOwner()->GetComponent<TransformComponent>();
        if (!tr)
            return;

        auto *camTr = ctx.camera->GetOwner()->GetTransform();
        glm::vec3 camPos = camTr ? camTr->GetWorldPosition() : glm::vec3(0.0f);

        // Get window size
        int winW = 0, winH = 0;
        SDL_GetRendererOutputSize(ctx.renderer, &winW, &winH);

        // Get world transform
        const glm::mat4 worldMatrix = tr->GetWorldMatrix();
        const glm::vec3 worldPos = glm::vec3(worldMatrix[3]);

        glm::vec3 basisX = glm::vec3(worldMatrix[0]);
        float scale = glm::length(basisX);

        float zoom = 1.0f;
        float screenX = (worldPos.x - camPos.x) * zoom + winW * 0.5f;
        float screenY = (worldPos.y - camPos.y) * zoom + winH * 0.5f;

        float screenRadius = radius_ * scale * zoom;

        SDL_SetRenderDrawColor(ctx.renderer, color_.r, color_.g, color_.b, color_.a);

        for (int w = 0; w < screenRadius * 2; w++)
        {
            for (int h = 0; h < screenRadius * 2; h++)
            {
                int dx = screenRadius - w; // horizontal offset
                int dy = screenRadius - h; // vertical offset
                if ((dx * dx + dy * dy) <= (screenRadius * screenRadius))
                {
                    SDL_RenderDrawPoint(ctx.renderer, static_cast<int>(screenX + dx - screenRadius), static_cast<int>(screenY + dy - screenRadius));
                }
            }
        }
    }

    void SetRadius(float r) { radius_ = r; }
    float GetRadius() const
    {
        const glm::vec3 s = GetOwner()->GetTransform()->GetScale();
        const float uniform = std::max(s.x, s.y);
        return radius_ * uniform;
    }

    void SetColor(const SDL_Color &color) { color_ = color; }
    SDL_Color GetColor() const { return color_; }

private:
    float radius_ = 50.0f;
    SDL_Color color_ = {255, 255, 255, 255}; // Default white
};