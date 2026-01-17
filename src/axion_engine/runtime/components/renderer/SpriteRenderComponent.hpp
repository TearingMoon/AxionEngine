#pragma once

#include <SDL.h>
#include <glm/glm.hpp>

#include "RenderComponent.hpp"
#include "axion_engine/runtime/components/transform/TransformComponent.hpp"

class SpriteRenderComponent : public RenderComponent
{
public:
    void SetTexture(SDL_Texture *tex) { texture_ = tex; }
    SDL_Texture *GetTexture() const { return texture_; }

    void SetSourceRect(const SDL_Rect &r)
    {
        srcRect_ = r;
        hasSrcRect_ = true;
    }

    void SetColor(const SDL_Color &col) { color_ = col; }
    void SetSize(const glm::vec2 &size) { size_ = size; }
    glm::vec2 GetSize() const { return size_; }
    SDL_Color GetColor() const { return color_; }
    bool HasSourceRect() const { return hasSrcRect_; }
    SDL_Rect GetSourceRect() const { return srcRect_; }

    void Render(const RenderContext &ctx) override
    {
        if (!texture_ || !ctx.sdl || !ctx.camera)
            return;

        auto *tr = GetOwner()->GetComponent<TransformComponent>();
        if (!tr)
            return;

        // Get camera transform
        auto *camTr = ctx.camera->GetOwner()->GetTransform();
        glm::vec3 camPos = camTr ? camTr->GetWorldPosition() : glm::vec3(0.0f);

        // Get window size
        int winW = 0, winH = 0;
        SDL_GetRendererOutputSize(ctx.sdl, &winW, &winH);

        // Get world transform
        const glm::mat4 worldMatrix = tr->GetWorldMatrix();
        const glm::vec3 worldPos = glm::vec3(worldMatrix[3]);

        // Extract scale and rotation from matrix
        glm::vec3 basisX = glm::vec3(worldMatrix[0]);
        glm::vec3 basisY = glm::vec3(worldMatrix[1]);
        float scaleX = glm::length(basisX);
        float scaleY = glm::length(basisY);

        float angleRad = std::atan2(worldMatrix[0][1], worldMatrix[0][0]);
        float angleDeg = -glm::degrees(angleRad);

        // World to screen conversion
        float zoom = 1.0f;
        float screenX = (worldPos.x - camPos.x) * zoom + winW * 0.5f;
        float screenY = winH * 0.5f - (worldPos.y - camPos.y) * zoom;

        // Setup source rect
        SDL_Rect src;
        if (hasSrcRect_)
        {
            src = srcRect_;
        }
        else
        {
            int texW = 0, texH = 0;
            SDL_QueryTexture(texture_, nullptr, nullptr, &texW, &texH);
            src = {0, 0, texW, texH};
        }

        // Calculate destination size
        glm::vec2 sizeWorld = size_ * glm::vec2(scaleX, scaleY);
        float dstW = (size_.x > 0.0f) ? sizeWorld.x : src.w * scaleX;
        float dstH = (size_.y > 0.0f) ? sizeWorld.y : src.h * scaleY;

        // Setup destination rect (centered on position)
        SDL_FRect dst;
        dst.w = dstW;
        dst.h = dstH;
        dst.x = screenX - dst.w * 0.5f;
        dst.y = screenY - dst.h * 0.5f;

        SDL_SetTextureColorMod(texture_, color_.r, color_.g, color_.b);
        SDL_SetTextureAlphaMod(texture_, color_.a);

        SDL_RenderCopyExF(
            ctx.sdl,
            texture_,
            &src,
            &dst,
            angleDeg,
            nullptr,
            SDL_FLIP_NONE);
    }

private:
    SDL_Texture *texture_ = nullptr;
    SDL_Rect srcRect_{0, 0, 0, 0};
    bool hasSrcRect_ = false;
    SDL_Color color_{255, 255, 255, 255};
    glm::vec2 size_{1.0f, 1.0f};
};