#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_ttf.h>
#include <glm/glm.hpp>

#include "axion_engine/managers/assets/AssetsManager.hpp"
#include "axion_engine/runtime/components/renderer/RenderComponent.hpp"
#include "axion_engine/runtime/interfaces/IRenderable.hpp"
#include "axion_engine/runtime/interfaces/IUpdateable.hpp"

namespace Axion
{

struct UIRenderContext
{
    SDL_Renderer* renderer = nullptr;
    AssetsManager* assets = nullptr;
    IInputProvider* input = nullptr;
    int width = 0;
    int height = 0;
};

class UIElement
{
public:
    virtual ~UIElement() = default;

    void SetPosition(glm::vec2 position) { position_ = position; }
    void SetSize(glm::vec2 size) { size_ = size; }
    void SetAnchor(glm::vec2 anchor) { anchor_ = anchor; }
    void SetVisible(bool visible) { visible_ = visible; }
    void SetEnabled(bool enabled) { enabled_ = enabled; }

    glm::vec2 GetPosition() const { return position_; }
    glm::vec2 GetSize() const { return size_; }
    bool IsVisible() const { return visible_; }
    bool IsEnabled() const { return enabled_; }
    SDL_Rect GetScreenRect(const UIRenderContext& context,
                           glm::vec2 parentOrigin = {}) const
    {
        return GetRect(context, parentOrigin);
    }

    virtual void Update(const UIRenderContext& context, glm::vec2 parentOrigin)
    {
        (void)context;
        (void)parentOrigin;
    }

    virtual void Render(const UIRenderContext& context, glm::vec2 parentOrigin) = 0;

protected:
    SDL_Rect GetRect(const UIRenderContext& context, glm::vec2 parentOrigin) const
    {
        const glm::vec2 anchored{
            anchor_.x * static_cast<float>(context.width),
            anchor_.y * static_cast<float>(context.height)};
        return {
            static_cast<int>(parentOrigin.x + anchored.x + position_.x),
            static_cast<int>(parentOrigin.y + anchored.y + position_.y),
            std::max(0, static_cast<int>(size_.x)),
            std::max(0, static_cast<int>(size_.y))};
    }

    bool Contains(const SDL_Rect& rect, SDL_Point point) const
    {
        return point.x >= rect.x && point.x < rect.x + rect.w &&
               point.y >= rect.y && point.y < rect.y + rect.h;
    }

    bool visible_ = true;
    bool enabled_ = true;
    glm::vec2 position_{0.0f};
    glm::vec2 size_{100.0f, 40.0f};
    glm::vec2 anchor_{0.0f};
};

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

    void Render(const UIRenderContext& context, glm::vec2 parentOrigin) override
    {
        if (!visible_ || !context.renderer || text_.empty())
            return;
        if (dirty_ || !texture_)
            Rebuild(context);
        if (!texture_)
            return;

        const SDL_Rect rect = GetRect(context, parentOrigin);
        SDL_Rect destination{rect.x, rect.y, rect.w, rect.h};
        if (destination.w <= 0)
            destination.w = textureWidth_;
        if (destination.h <= 0)
            destination.h = textureHeight_;
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
    bool dirty_ = true;
};

class UIPanel final : public UIElement
{
public:
    void SetColor(SDL_Color color) { color_ = color; }

    template <typename T, typename... Args>
    T* Add(Args&&... args)
    {
        auto element = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw = element.get();
        children_.push_back(std::move(element));
        return raw;
    }

    void Update(const UIRenderContext& context, glm::vec2 parentOrigin) override
    {
        if (!visible_ || !enabled_)
            return;
        const SDL_Rect rect = GetRect(context, parentOrigin);
        const glm::vec2 origin{static_cast<float>(rect.x), static_cast<float>(rect.y)};
        for (auto& child : children_)
            child->Update(context, origin);
    }

    void Render(const UIRenderContext& context, glm::vec2 parentOrigin) override
    {
        if (!visible_)
            return;
        const SDL_Rect rect = GetRect(context, parentOrigin);
        SDL_SetRenderDrawBlendMode(context.renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(context.renderer, color_.r, color_.g, color_.b, color_.a);
        SDL_RenderFillRect(context.renderer, &rect);
        const glm::vec2 origin{static_cast<float>(rect.x), static_cast<float>(rect.y)};
        for (auto& child : children_)
            child->Render(context, origin);
    }

private:
    SDL_Color color_{20, 25, 40, 220};
    std::vector<std::unique_ptr<UIElement>> children_;
};

class UIButton final : public UIElement
{
public:
    using Callback = std::function<void()>;

    explicit UIButton(std::string text) : label_(std::move(text))
    {
        label_.SetFontAsset("default");
        label_.SetPosition({8.0f, 4.0f});
        label_.SetSize({0.0f, 0.0f});
    }

    void SetOnClick(Callback callback) { onClick_ = std::move(callback); }
    void SetColors(SDL_Color normal, SDL_Color hovered, SDL_Color pressed)
    {
        normal_ = normal;
        hovered_ = hovered;
        pressed_ = pressed;
    }
    bool IsHovered() const { return hoveredState_; }

    void Update(const UIRenderContext& context, glm::vec2 parentOrigin) override
    {
        if (!visible_ || !enabled_ || !context.input)
            return;
        const SDL_Rect rect = GetRect(context, parentOrigin);
        hoveredState_ = Contains(rect, context.input->GetMousePosition());
        pressedState_ = hoveredState_ &&
                        context.input->IsMouseButtonDown(SDL_BUTTON_LEFT);
        if (hoveredState_ &&
            context.input->IsMouseButtonJustPressed(SDL_BUTTON_LEFT) &&
            onClick_)
        {
            context.input->ConsumeMouseButton(SDL_BUTTON_LEFT);
            onClick_();
        }
    }

    void Render(const UIRenderContext& context, glm::vec2 parentOrigin) override
    {
        if (!visible_ || !context.renderer)
            return;
        const SDL_Rect rect = GetRect(context, parentOrigin);
        const SDL_Color color = pressedState_ ? pressed_ :
                                hoveredState_ ? hovered_ : normal_;
        SDL_SetRenderDrawBlendMode(context.renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(context.renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(context.renderer, &rect);
        SDL_SetRenderDrawColor(context.renderer, 220, 230, 245, 255);
        SDL_RenderDrawRect(context.renderer, &rect);
        label_.Render(context, {static_cast<float>(rect.x),
                                static_cast<float>(rect.y)});
    }

private:
    UILabel label_;
    Callback onClick_;
    SDL_Color normal_{45, 65, 95, 255};
    SDL_Color hovered_{65, 105, 155, 255};
    SDL_Color pressed_{35, 45, 70, 255};
    bool hoveredState_ = false;
    bool pressedState_ = false;
};

class Canvas final : public RenderComponent, public IUpdateable
{
public:
    Canvas() { SetLayer(RenderLayer::UI); }

    template <typename T, typename... Args>
    T* Add(Args&&... args)
    {
        auto element = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw = element.get();
        children_.push_back(std::move(element));
        return raw;
    }

    void ProcessInput(EngineContext& context)
    {
        assets_ = context.assets;
        input_ = context.inputProvider;
        if (!context.window || !context.inputProvider)
            return;
        int width = 0;
        int height = 0;
        SDL_GetRendererOutputSize(context.window->GetRenderer(), &width, &height);
        UIRenderContext ui{context.window->GetRenderer(), context.assets,
                           context.inputProvider, width, height};
        for (auto& child : children_)
            child->Update(ui, {});
    }

    void Update(EngineContext& context) override
    {
        (void)context;
    }

    void Render(const RenderContext& context) override
    {
        UIRenderContext ui{context.renderer, context.assets, context.input,
                           context.windowWidth, context.windowHeight};
        for (auto& child : children_)
            child->Render(ui, {});
    }

private:
    AssetsManager* assets_ = nullptr;
    IInputProvider* input_ = nullptr;
    std::vector<std::unique_ptr<UIElement>> children_;
};

} // namespace Axion
