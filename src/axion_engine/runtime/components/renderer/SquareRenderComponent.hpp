#pragma once
#include "RenderComponent.hpp"
#include "axion_engine/runtime/components/transform/TransformComponent.hpp"

namespace Axion
{

/**
 * @brief Solid colored square rendering component.
 *
 * Renders a filled, rotatable square using SDL geometry rendering.
 */
class SquareRenderComponent : public RenderComponent
{
public:
    void Render(const RenderContext& ctx) override
    {
        if (!ctx.renderer)
            return;

        auto* tr = GetOwner()->GetComponent<TransformComponent>();
        if (!tr)
            return;

        auto* camTr = ctx.camera->GetOwner()->GetTransform();
        glm::vec3 camPos = camTr ? camTr->GetWorldPosition() : glm::vec3(0.0f);

        int winW = 0, winH = 0;
        SDL_GetRendererOutputSize(ctx.renderer, &winW, &winH);

        const glm::mat4 worldMatrix = tr->GetWorldMatrix();
        const glm::vec3 worldPos = glm::vec3(worldMatrix[3]);

        glm::vec3 basisX = glm::vec3(worldMatrix[0]);
        glm::vec3 basisY = glm::vec3(worldMatrix[1]);
        float scaleX = glm::length(basisX);
        float scaleY = glm::length(basisY);

        float rotation = glm::radians(tr->GetRotation().z);

        float zoom = 1.0f;
        float screenX = (worldPos.x - camPos.x) * zoom + winW * 0.5f;
        float screenY = (worldPos.y - camPos.y) * zoom + winH * 0.5f;

        float halfW = (scaleX * zoom) * 0.5f;
        float halfH = (scaleY * zoom) * 0.5f;

        float cosR = std::cos(rotation);
        float sinR = std::sin(rotation);

        glm::vec2 corners[4] = {
            {-halfW, -halfH},
            {halfW, -halfH},
            {halfW, halfH},
            {-halfW, halfH}
        };

        SDL_Vertex vertices[4];
        for (int i = 0; i < 4; ++i)
        {
            float rotX = corners[i].x * cosR - corners[i].y * sinR;
            float rotY = corners[i].x * sinR + corners[i].y * cosR;

            vertices[i].position.x = screenX + rotX;
            vertices[i].position.y = screenY + rotY;
            vertices[i].color = color_;
        }

        int indices[6] = {0, 1, 2, 0, 2, 3};

        SDL_RenderGeometry(ctx.renderer, nullptr, vertices, 4, indices, 6);
    }

    /**
     * @brief Set the fill color.
     * @param color RGBA color.
     */
    void SetColor(const SDL_Color& color) { color_ = color; }

private:
    SDL_Color color_ = {255, 0, 0, 255};
};

} // namespace Axion