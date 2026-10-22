#pragma once
#include <string>
#include <array>
#include <cmath>

#include <SDL.h>
#include <SDL_ttf.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "RenderComponent.hpp"
#include "axion_engine/managers/assets/AssetsManager.hpp"
#include "axion_engine/runtime/components/transform/TransformComponent.hpp"

namespace Axion
{
/**
 * @brief Text rendering component using SDL_ttf.
 *
 * Renders UTF-8 text with configurable font, color, and size.
 * Caches the rendered texture for performance.
 */
class TextRenderComponent : public RenderComponent
{
public:
    ~TextRenderComponent() override
    {
        DestroyTexture();
        if (ownedFont_)
        {
            TTF_CloseFont(ownedFont_);
            ownedFont_ = nullptr;
        }
    }

    /**
     * @brief Set the text to display.
     * @param text UTF-8 encoded string.
     */
    void SetText(const std::string& text)
    {
        if (text_ == text)
            return;
        text_ = text;
        dirty_ = true;
    }

    /**
     * @brief Set the text color.
     * @param color RGBA color.
     */
    void SetColor(const SDL_Color& color)
    {
        color_ = color;
        dirty_ = true;
    }

    /**
     * @brief Set an external font.
     * @param font TTF_Font pointer (ownership not transferred).
     */
    void SetFont(TTF_Font* font)
    {
        font_ = font;
        fontAssetId_.clear();
        dirty_ = true;
    }

    void SetFontAsset(const std::string& id,
                      const std::string& relativePath = "fonts/default.ttf")
    {
        fontAssetId_ = id;
        fontAssetPath_ = relativePath;
        if (ownedFont_)
        {
            TTF_CloseFont(ownedFont_);
            ownedFont_ = nullptr;
        }
        font_ = nullptr;
        dirty_ = true;
    }

    /**
     * @brief Set the font size for auto-loaded fonts.
     * @param size Font size in points.
     */
    void SetFontSize(int size)
    {
        fontSize_ = std::max(1, size);
        if (loadedFontSize_ != fontSize_)
        {
            if (ownedFont_)
                TTF_CloseFont(ownedFont_);
            ownedFont_ = nullptr;
            font_ = nullptr;
            loadedFontSize_ = 0;
        }
        dirty_ = true;
    }

    void Render(const RenderContext& ctx) override
    {
        if (!ctx.renderer || !ctx.camera)
            return;

        auto* tr = GetOwner()->GetComponent<TransformComponent>();
        if (!tr)
            return;

        if (!font_ && !ownedFont_ && ctx.assets && !fontAssetId_.empty())
        {
            font_ = ctx.assets->LoadFont(
                fontAssetId_, fontAssetPath_, fontSize_);
        }

        if (!font_ && !ownedFont_)
        {
            const std::array<const char*, 8> fontPaths = {
                "C:/Windows/Fonts/arial.ttf",
                "C:/Windows/Fonts/calibri.ttf",
                "C:/Windows/Fonts/segoeui.ttf",
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf",
                "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
                "/System/Library/Fonts/Supplemental/Arial.ttf",
                "/Library/Fonts/Arial.ttf"
            };

            for (const char* path : fontPaths)
            {
                ownedFont_ = TTF_OpenFont(path, fontSize_);
                if (ownedFont_)
                {
                    font_ = ownedFont_;
                    loadedFontSize_ = fontSize_;
                    break;
                }
            }

            if (font_)
                loadedFontSize_ = fontSize_;

            if (!font_)
            {
                SDL_LogError(SDL_LOG_CATEGORY_RENDER,
                             "TextRenderComponent could not load a font: %s",
                             TTF_GetError());
                return;
            }
        }

        if (!font_ || text_.empty())
            return;

        if (dirty_ || !texture_)
        {
            if (!RebuildTexture(ctx.renderer))
                return;
        }

        int winW = 0, winH = 0;
        SDL_GetRendererOutputSize(ctx.renderer, &winW, &winH);

        const glm::vec3 worldPos = tr->GetWorldPosition();
        const float angleDeg = tr->GetWorldRotation().z;

        const glm::vec3 worldScale = tr->GetWorldScale();
        const float sx = worldScale.x;
        const float sy = worldScale.y;

        const glm::vec2 screenPosition =
            ctx.camera->WorldToScreen(worldPos, winW, winH);
        const glm::vec2 screenSize = ctx.camera->WorldSizeToScreen(
            worldPos, {static_cast<float>(texW_) * sx,
                       static_cast<float>(texH_) * sy},
            winW, winH);

        SDL_FRect dst;
        dst.w = screenSize.x;
        dst.h = screenSize.y;
        dst.x = screenPosition.x - dst.w * 0.5f;
        dst.y = screenPosition.y - dst.h * 0.5f;

        SDL_FPoint pivot;
        pivot.x = dst.w * 0.5f;
        pivot.y = dst.h * 0.5f;

        SDL_RenderCopyExF(
            ctx.renderer,
            texture_,
            nullptr,
            &dst,
            static_cast<double>(angleDeg),
            &pivot,
            SDL_FLIP_NONE);
    }

private:
    bool RebuildTexture(SDL_Renderer* renderer)
    {
        DestroyTexture();

        SDL_Surface* surf = TTF_RenderUTF8_Blended(font_, text_.c_str(), color_);
        if (!surf)
            return false;

        texture_ = SDL_CreateTextureFromSurface(renderer, surf);
        texW_ = static_cast<float>(surf->w);
        texH_ = static_cast<float>(surf->h);

        SDL_FreeSurface(surf);

        if (!texture_)
            return false;

        SDL_SetTextureBlendMode(texture_, SDL_BLENDMODE_BLEND);

        dirty_ = false;
        return true;
    }

    void DestroyTexture()
    {
        if (texture_)
        {
            SDL_DestroyTexture(texture_);
            texture_ = nullptr;
        }
        texW_ = 0.0f;
        texH_ = 0.0f;
    }

private:
    std::string text_;
    SDL_Color color_{255, 255, 255, 255};
    int fontSize_ = 16;

    TTF_Font* font_ = nullptr;
    TTF_Font* ownedFont_ = nullptr;
    std::string fontAssetId_ = "default";
    std::string fontAssetPath_ = "fonts/default.ttf";
    int loadedFontSize_ = 0;

    SDL_Texture* texture_ = nullptr;
    float texW_ = 0.0f;
    float texH_ = 0.0f;
    bool dirty_ = true;
};

} // namespace Axion
