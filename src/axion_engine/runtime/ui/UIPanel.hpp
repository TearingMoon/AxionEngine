#pragma once

#include <memory>
#include <vector>

#include "UIElement.hpp"

namespace Axion
{

class UIPanel final : public UIElement
{
public:
    void SetColor(SDL_Color color) { color_ = color; }
    void SetPadding(float padding) { padding_ = std::max(0.0f, padding); }
    void SetChildAlignment(UIHorizontalAlignment horizontal,
                           UIVerticalAlignment vertical)
    {
        horizontalAlignment_ = horizontal;
        verticalAlignment_ = vertical;
    }

    template <typename T, typename... Args>
    T* Add(Args&&... args)
    {
        auto element = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw = element.get();
        children_.push_back(std::move(element));
        return raw;
    }

    void Update(const UIRenderContext& context, SDL_Rect parentRect) override
    {
        if (!visible_ || !enabled_)
            return;
        const SDL_Rect rect = GetRect(parentRect);
        const SDL_Rect content = GetContentRect(rect);
        for (auto& child : children_)
        {
            LayoutChild(*child, content);
            child->Update(context, content);
        }
    }

    void Render(const UIRenderContext& context, SDL_Rect parentRect) override
    {
        if (!visible_)
            return;
        const SDL_Rect rect = GetRect(parentRect);
        SDL_SetRenderDrawBlendMode(context.renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(context.renderer, color_.r, color_.g, color_.b, color_.a);
        SDL_RenderFillRect(context.renderer, &rect);
        const SDL_Rect content = GetContentRect(rect);
        for (auto& child : children_)
        {
            LayoutChild(*child, content);
            child->Render(context, content);
        }
    }

private:
    SDL_Rect GetContentRect(SDL_Rect rect) const
    {
        const int padding = static_cast<int>(padding_);
        return {rect.x + padding, rect.y + padding,
                std::max(0, rect.w - padding * 2),
                std::max(0, rect.h - padding * 2)};
    }

    void LayoutChild(UIElement& child, SDL_Rect content)
    {
        if (!child.IsAutoLayout())
            return;
        const glm::vec2 size = child.GetSize();
        float x = static_cast<float>(content.x);
        float y = static_cast<float>(content.y);
        if (horizontalAlignment_ == UIHorizontalAlignment::Center)
            x += (content.w - size.x) * 0.5f;
        else if (horizontalAlignment_ == UIHorizontalAlignment::Right)
            x += content.w - size.x;
        if (verticalAlignment_ == UIVerticalAlignment::Center)
            y += (content.h - size.y) * 0.5f;
        else if (verticalAlignment_ == UIVerticalAlignment::Bottom)
            y += content.h - size.y;
        child.SetAnchor({});
        child.SetPosition({x - content.x, y - content.y});
    }

    SDL_Color color_{20, 25, 40, 220};
    float padding_ = 0.0f;
    UIHorizontalAlignment horizontalAlignment_ = UIHorizontalAlignment::Left;
    UIVerticalAlignment verticalAlignment_ = UIVerticalAlignment::Top;
    std::vector<std::unique_ptr<UIElement>> children_;
};

} // namespace Axion
