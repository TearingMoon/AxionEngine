#pragma once
#include <string>
#include <cmath>

#include <SDL.h>
#include <SDL_ttf.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "RenderComponent.hpp"
#include "axion_engine/runtime/components/transform/TransformComponent.hpp"

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

    void SetText(const std::string &text)
    {
        if (text_ == text)
            return;
        text_ = text;
        dirty_ = true;
    }

    void SetColor(const SDL_Color &color)
    {
        color_ = color;
        dirty_ = true;
    }

    void SetFont(TTF_Font *font)
    {
        font_ = font;
        dirty_ = true;
    }

    // If you load fonts externally, don't do font-size here.
    // Keep this if you manage a font cache by size.
    void SetFontSize(int size)
    {
        fontSize_ = size;
        // If you use a font cache, you'd swap font_ here and set dirty_ = true.
        dirty_ = true;
    }

    void Render(const RenderContext &ctx) override
    {
        if (!ctx.renderer || !ctx.camera)
            return;

        auto *tr = GetOwner()->GetComponent<TransformComponent>();
        if (!tr)
            return;

        // Auto-load default font if none is set
        if (!font_ && !ownedFont_)
        {
            // Try to load a default system font
            // Common paths for Windows fonts
            const char* fontPaths[] = {
                "C:/Windows/Fonts/arial.ttf",
                "C:/Windows/Fonts/calibri.ttf",
                "C:/Windows/Fonts/segoeui.ttf"
            };
            
            for (const char* path : fontPaths)
            {
                ownedFont_ = TTF_OpenFont(path, fontSize_);
                if (ownedFont_)
                {
                    font_ = ownedFont_;
                    break;
                }
            }
            
            if (!font_)
            {
                // If still no font, can't render
                return;
            }
        }

        if (!font_ || text_.empty())
            return;

        // Rebuild texture only when needed
        if (dirty_ || !texture_)
        {
            if (!RebuildTexture(ctx.renderer))
                return;
        }

        // Get camera position (world)
        auto *camTr = ctx.camera->GetOwner()->GetTransform();
        const glm::vec3 camPos = camTr ? camTr->GetWorldPosition() : glm::vec3(0.0f);

        // Get window size
        int winW = 0, winH = 0;
        SDL_GetRendererOutputSize(ctx.renderer, &winW, &winH);

        // World position
        const glm::vec3 worldPos = tr->GetWorldPosition();

        // Rotation around Z (degrees for SDL)
        const float angleDeg = tr->GetWorldRotation().z;

        // Scale (assume uniform-ish in 2D; use X/Y separately if you want)
        const glm::vec3 worldScale = tr->GetWorldScale();
        const float sx = worldScale.x;
        const float sy = worldScale.y;

        // World -> screen (camera centered). If your Y-axis is up in world,
        // you likely want to invert Y for screen. Your previous code didn't invert;
        // I'll keep it consistent with your snippet (no inversion).
        const float zoom = 1.0f;
        const float screenX = (worldPos.x - camPos.x) * zoom + winW * 0.5f;
        const float screenY = (worldPos.y - camPos.y) * zoom + winH * 0.5f;

        // Destination rect (centered)
        SDL_FRect dst;
        dst.w = texW_ * sx;
        dst.h = texH_ * sy;
        dst.x = screenX - dst.w * 0.5f;
        dst.y = screenY - dst.h * 0.5f;

        // Rotate around the center of the text quad
        SDL_FPoint pivot;
        pivot.x = dst.w * 0.5f;
        pivot.y = dst.h * 0.5f;

        SDL_RenderCopyExF(
            ctx.renderer,
            texture_,
            nullptr,
            &dst,
            (double)angleDeg,
            &pivot,
            SDL_FLIP_NONE);
    }

private:
    bool RebuildTexture(SDL_Renderer *renderer)
    {
        DestroyTexture();

        // Render text to surface
        SDL_Surface *surf = TTF_RenderUTF8_Blended(font_, text_.c_str(), color_);
        if (!surf)
            return false;

        texture_ = SDL_CreateTextureFromSurface(renderer, surf);
        texW_ = (float)surf->w;
        texH_ = (float)surf->h;

        SDL_FreeSurface(surf);

        if (!texture_)
            return false;

        // Enable alpha blending
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

    // External font (recommended): set via SetFont()
    TTF_Font *font_ = nullptr;
    TTF_Font *ownedFont_ = nullptr; // Font owned by this component

    // Cached texture
    SDL_Texture *texture_ = nullptr;
    float texW_ = 0.0f;
    float texH_ = 0.0f;
    bool dirty_ = true;
};
