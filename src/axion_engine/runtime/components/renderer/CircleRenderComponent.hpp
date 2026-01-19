#pragma once
#include "RenderComponent.hpp"
#include "axion_engine/runtime/components/transform/TransformComponent.hpp"

namespace Axion
{

/**
 * @brief Solid colored circle rendering component.
 *
 * Renders a filled circle using point-based rendering.
 */
class CircleRenderComponent : public RenderComponent
{
public:
    void Render(const RenderContext& ctx) override
    {
        if (!ctx.renderer)
            return;

        auto* owner = GetOwner();
        if (!owner || owner->IsDestroyed())
            return;

        auto* tr = owner->GetComponent<TransformComponent>();
        if (!tr)
            return;

        if (!ctx.camera)
            return;

        auto* camOwner = ctx.camera->GetOwner();
        if (!camOwner || camOwner->IsDestroyed())
            return;

        auto* camTr = camOwner->GetTransform();
        glm::vec3 camPos = camTr ? camTr->GetWorldPosition() : glm::vec3(0.0f);

        int winW = 0, winH = 0;
        SDL_GetRendererOutputSize(ctx.renderer, &winW, &winH);

        const glm::mat4 worldMatrix = tr->GetWorldMatrix();
        const glm::vec3 worldPos = glm::vec3(worldMatrix[3]);

        glm::vec3 basisX = glm::vec3(worldMatrix[0]);
        float scale = glm::length(basisX);

        float zoom = 1.0f;
        float screenX = (worldPos.x - camPos.x) * zoom + winW * 0.5f;
        float screenY = winH * 0.5f - (worldPos.y - camPos.y) * zoom;

        float screenRadius = radius_ * scale * zoom;

        SDL_SetRenderDrawColor(ctx.renderer, color_.r, color_.g, color_.b, color_.a);

        for (int w = 0; w < screenRadius * 2; w++)
        {
            for (int h = 0; h < screenRadius * 2; h++)
            {
                int dx = w - static_cast<int>(screenRadius);
                int dy = h - static_cast<int>(screenRadius);
                if ((dx * dx + dy * dy) <= static_cast<int>(screenRadius * screenRadius))
                {
                    SDL_RenderDrawPoint(ctx.renderer, static_cast<int>(screenX + dx), static_cast<int>(screenY + dy));
                }
            }
        }
    }

    /**
     * @brief Set the base radius.
     * @param r Radius value.
     */
    void SetRadius(float r) { radius_ = r; }

    /**
     * @brief Get the effective radius (scaled by transform).
     * @return The scaled radius.
     */
    float GetRadius() const
    {
        auto* owner = GetOwner();
        if (!owner) return radius_;
        auto* tr = owner->GetTransform();
        if (!tr) return radius_;
        const glm::vec3 s = tr->GetScale();
        const float uniform = std::max(s.x, s.y);
        return radius_ * uniform;
    }

    /**
     * @brief Set the fill color.
     * @param color RGBA color.
     */
    void SetColor(const SDL_Color& color) { color_ = color; }

    SDL_Color GetColor() const { return color_; }

private:
    float radius_ = 50.0f;
    SDL_Color color_ = {255, 255, 255, 255};
};

} // namespace Axion