#pragma once

#include <SDL.h>
#include <glm/glm.hpp>

#include "RenderComponent.hpp"
#include "axion_engine/runtime/components/transform/TransformComponent.hpp"

namespace Axion
{

/**
 * @brief Sprite rendering component for textured 2D objects.
 *
 * Renders an SDL texture with support for source rectangles,
 * color modulation, custom sizing, and rotation.
 */
class SpriteRenderComponent : public RenderComponent
{
public:
    /**
     * @brief Set the texture to render.
     * @param tex SDL texture pointer (ownership not transferred).
     */
    void SetTexture(SDL_Texture* tex) { texture_ = tex; }

    /**
     * @brief Get the current texture.
     * @return The SDL texture pointer.
     */
    SDL_Texture* GetTexture() const { return texture_; }

    /**
     * @brief Set a source rectangle for texture sampling.
     * @param r The source rectangle in texture coordinates.
     */
    void SetSourceRect(const SDL_Rect& r)
    {
        srcRect_ = r;
        hasSrcRect_ = true;
    }

    /**
     * @brief Set the color modulation.
     * @param col RGBA color to apply.
     */
    void SetColor(const SDL_Color& col) { color_ = col; }

    /**
     * @brief Set the display size in world units.
     * @param size Width and height.
     */
    void SetSize(const glm::vec2& size) { size_ = size; }

    glm::vec2 GetSize() const { return size_; }
    SDL_Color GetColor() const { return color_; }
    bool HasSourceRect() const { return hasSrcRect_; }
    SDL_Rect GetSourceRect() const { return srcRect_; }

    void Render(const RenderContext& ctx) override
    {
        if (!texture_ || !ctx.renderer || !ctx.camera)
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

        float angleRad = std::atan2(worldMatrix[0][1], worldMatrix[0][0]);
        float angleDeg = -glm::degrees(angleRad);

        float zoom = 1.0f;
        float screenX = (worldPos.x - camPos.x) * zoom + winW * 0.5f;
        float screenY = winH * 0.5f - (worldPos.y - camPos.y) * zoom;

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

        glm::vec2 sizeWorld = size_ * glm::vec2(scaleX, scaleY);
        float dstW = (size_.x > 0.0f) ? sizeWorld.x : src.w * scaleX;
        float dstH = (size_.y > 0.0f) ? sizeWorld.y : src.h * scaleY;

        SDL_FRect dst;
        dst.w = dstW;
        dst.h = dstH;
        dst.x = screenX - dst.w * 0.5f;
        dst.y = screenY - dst.h * 0.5f;

        SDL_SetTextureColorMod(texture_, color_.r, color_.g, color_.b);
        SDL_SetTextureAlphaMod(texture_, color_.a);

        SDL_RenderCopyExF(
            ctx.renderer,
            texture_,
            &src,
            &dst,
            angleDeg,
            nullptr,
            SDL_FLIP_NONE);
    }

private:
    SDL_Texture* texture_ = nullptr;
    SDL_Rect srcRect_{0, 0, 0, 0};
    bool hasSrcRect_ = false;
    SDL_Color color_{255, 255, 255, 255};
    glm::vec2 size_{1.0f, 1.0f};
};

} // namespace Axion