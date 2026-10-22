#pragma once

#include <array>
#include <string>

#include <SDL_ttf.h>

#include "UIElement.hpp"

namespace Axion
{

class UILabel final : public UIElement
{
public:
    explicit UILabel(std::string text) : text_(std::move(text)) {}

    void SetText(std::string text) { text_ = std::move(text); dirty_ = true; }
    void SetColor(SDL_Color color) { color_ = color; dirty_ = true; }
    void SetFontAsset(std::string id, std::string path = "fonts/default.ttf")
    {
        fontId_ = std::move(id);
        fontPath_ = std::move(path);
        dirty_ = true;
    }
    void SetFontSize(int size) { fontSize_ = std::max(1, size); dirty_ = true; }
    void SetAlignment(UIHorizontalAlignment horizontal,
                      UIVerticalAlignment vertical)
    {
        horizontalAlignment_ = horizontal;
        verticalAlignment_ = vertical;
    }

    void Render(const UIRenderContext& context, SDL_Rect parentRect) override
    {
        if (!visible_ || !context.renderer || text_.empty())
            return;
        if (dirty_ || !texture_)
            Rebuild(context);
        if (!texture_)
            return;

        const SDL_Rect rect = GetRect(parentRect);
        SDL_Rect destination{rect.x, rect.y, textureWidth_, textureHeight_};
        if (rect.w > 0)
        {
            if (horizontalAlignment_ == UIHorizontalAlignment::Center)
                destination.x += (rect.w - destination.w) / 2;
            else if (horizontalAlignment_ == UIHorizontalAlignment::Right)
                destination.x += rect.w - destination.w;
        }
        if (rect.h > 0)
        {
            if (verticalAlignment_ == UIVerticalAlignment::Center)
                destination.y += (rect.h - destination.h) / 2;
            else if (verticalAlignment_ == UIVerticalAlignment::Bottom)
                destination.y += rect.h - destination.h;
        }
        SDL_RenderCopy(context.renderer, texture_, nullptr, &destination);
    }

    ~UILabel() override
    {
        if (texture_)
            SDL_DestroyTexture(texture_);
        if (ownedFont_)
            TTF_CloseFont(ownedFont_);
    }

private:
    void Rebuild(const UIRenderContext& context)
    {
        TTF_Font* font = nullptr;
        if (context.assets && !fontId_.empty())
            font = context.assets->LoadFont(fontId_, fontPath_, fontSize_);
        if (!font)
            font = LoadSystemFont();
        if (!font)
            return;

        SDL_Surface* surface =
            TTF_RenderUTF8_Blended(font, text_.c_str(), color_);
        if (!surface)
            return;

        SDL_Texture* newTexture =
            SDL_CreateTextureFromSurface(context.renderer, surface);
        textureWidth_ = surface->w;
        textureHeight_ = surface->h;
        SDL_FreeSurface(surface);
        if (!newTexture)
            return;

        if (texture_)
            SDL_DestroyTexture(texture_);
        texture_ = newTexture;
        dirty_ = false;
    }

    TTF_Font* LoadSystemFont()
    {
        if (ownedFont_)
            return ownedFont_;
        const std::array<const char*, 6> paths = {
            "C:/Windows/Fonts/arial.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf",
            "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
            "/System/Library/Fonts/Supplemental/Arial.ttf",
            "/Library/Fonts/Arial.ttf"};
        for (const char* path : paths)
        {
            ownedFont_ = TTF_OpenFont(path, fontSize_);
            if (ownedFont_)
                return ownedFont_;
        }
        return nullptr;
    }

    std::string text_;
    std::string fontId_ = "default";
    std::string fontPath_ = "fonts/default.ttf";
    SDL_Color color_{255, 255, 255, 255};
    int fontSize_ = 18;
    SDL_Texture* texture_ = nullptr;
    TTF_Font* ownedFont_ = nullptr;
    int textureWidth_ = 0;
    int textureHeight_ = 0;
    UIHorizontalAlignment horizontalAlignment_ = UIHorizontalAlignment::Left;
    UIVerticalAlignment verticalAlignment_ = UIVerticalAlignment::Top;
    bool dirty_ = true;
};

} // namespace Axion
