#pragma once
#include "RenderComponent.hpp"
#include "axion_engine/runtime/components/transform/TransformComponent.hpp"

class SquareRenderComponent : public RenderComponent
{
public:
    void Render(const RenderContext &ctx) override
    {
        // Implementation for rendering a square
        if (!ctx.renderer)
            return;

        auto *tr = GetOwner()->GetComponent<TransformComponent>();
        if (!tr)
            return;

        // Get camera transform
        auto *camTr = ctx.camera->GetOwner()->GetTransform();
        glm::vec3 camPos = camTr ? camTr->GetWorldPosition() : glm::vec3(0.0f);

        // Get window size
        int winW = 0, winH = 0;
        SDL_GetRendererOutputSize(ctx.renderer, &winW, &winH);

        // Get world transform
        const glm::mat4 worldMatrix = tr->GetWorldMatrix();
        const glm::vec3 worldPos = glm::vec3(worldMatrix[3]);

        // Extract scale and rotation from matrix
        glm::vec3 basisX = glm::vec3(worldMatrix[0]);
        glm::vec3 basisY = glm::vec3(worldMatrix[1]);
        float scaleX = glm::length(basisX);
        float scaleY = glm::length(basisY);
        
        // Extract rotation (in radians)
        float rotation = glm::radians(tr->GetRotation().z); // Convert degrees to radians

        // World to screen conversion
        float zoom = 1.0f;
        float screenX = (worldPos.x - camPos.x) * zoom + winW * 0.5f;
        float screenY = (worldPos.y - camPos.y) * zoom + winH * 0.5f;

        float halfW = (scaleX * zoom) * 0.5f;
        float halfH = (scaleY * zoom) * 0.5f;

        // Calculate rotated corners
        float cosR = std::cos(rotation);
        float sinR = std::sin(rotation);

        glm::vec2 corners[4] = {
            {-halfW, -halfH}, // Top-left
            { halfW, -halfH}, // Top-right
            { halfW,  halfH}, // Bottom-right
            {-halfW,  halfH}  // Bottom-left
        };

        SDL_Vertex vertices[4];
        for (int i = 0; i < 4; ++i)
        {
            // Rotate corner
            float rotX = corners[i].x * cosR - corners[i].y * sinR;
            float rotY = corners[i].x * sinR + corners[i].y * cosR;
            
            vertices[i].position.x = screenX + rotX;
            vertices[i].position.y = screenY + rotY;
            vertices[i].color = {255, 0, 0, 255}; // Red color
        }

        // Define indices for two triangles
        int indices[6] = {0, 1, 2, 0, 2, 3};

        SDL_RenderGeometry(ctx.renderer, nullptr, vertices, 4, indices, 6);
    }
};