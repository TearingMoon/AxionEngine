#pragma once

#include <algorithm>

#include <SDL.h>
#include <glm/glm.hpp>

#include "axion_engine/managers/assets/AssetsManager.hpp"

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

enum class UIHorizontalAlignment
{
    Left,
    Center,
    Right
};

enum class UIVerticalAlignment
{
    Top,
    Center,
    Bottom
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
    void SetAutoLayout(bool enabled) { autoLayout_ = enabled; }
    void SetFillParent(bool enabled) { fillParent_ = enabled; }

    glm::vec2 GetPosition() const { return position_; }
    glm::vec2 GetSize() const { return size_; }
    bool IsVisible() const { return visible_; }
    bool IsEnabled() const { return enabled_; }
    bool IsAutoLayout() const { return autoLayout_; }

    SDL_Rect GetScreenRect(const UIRenderContext& context,
                           SDL_Rect parentRect = {}) const
    {
        if (parentRect.w <= 0)
            parentRect = {0, 0, context.width, context.height};
        return GetRect(parentRect);
    }

    virtual void Update(const UIRenderContext& context, SDL_Rect parentRect)
    {
        (void)context;
        (void)parentRect;
    }

    virtual void Render(const UIRenderContext& context, SDL_Rect parentRect) = 0;

protected:
    SDL_Rect GetRect(SDL_Rect parentRect) const
    {
        if (fillParent_)
            return {parentRect.x + static_cast<int>(position_.x),
                    parentRect.y + static_cast<int>(position_.y),
                    std::max(0, parentRect.w - static_cast<int>(position_.x)),
                    std::max(0, parentRect.h - static_cast<int>(position_.y))};
        const glm::vec2 anchored{
            anchor_.x * static_cast<float>(parentRect.w),
            anchor_.y * static_cast<float>(parentRect.h)};
        return {
            static_cast<int>(parentRect.x + anchored.x + position_.x),
            static_cast<int>(parentRect.y + anchored.y + position_.y),
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
    bool autoLayout_ = false;
    bool fillParent_ = false;
    glm::vec2 position_{0.0f};
    glm::vec2 size_{100.0f, 40.0f};
    glm::vec2 anchor_{0.0f};
};

} // namespace Axion
