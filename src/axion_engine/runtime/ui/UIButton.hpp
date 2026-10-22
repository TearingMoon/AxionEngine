#pragma once

#include <functional>
#include <string>

#include "UILabel.hpp"

namespace Axion
{

class UIButton final : public UIElement
{
public:
    using Callback = std::function<void()>;

    explicit UIButton(std::string text) : label_(std::move(text))
    {
        label_.SetFontAsset("default");
    }

    void SetOnClick(Callback callback) { onClick_ = std::move(callback); }
    void SetColors(SDL_Color normal, SDL_Color hovered, SDL_Color pressed)
    {
        normal_ = normal;
        hovered_ = hovered;
        pressed_ = pressed;
    }
    bool IsHovered() const { return hoveredState_; }

    void Update(const UIRenderContext& context, SDL_Rect parentRect) override
    {
        if (!visible_ || !enabled_ || !context.input)
            return;
        const SDL_Rect rect = GetRect(parentRect);
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

    void Render(const UIRenderContext& context, SDL_Rect parentRect) override
    {
        if (!visible_ || !context.renderer)
            return;
        const SDL_Rect rect = GetRect(parentRect);
        const SDL_Color color = pressedState_ ? pressed_ :
                                hoveredState_ ? hovered_ : normal_;
        SDL_SetRenderDrawBlendMode(context.renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(context.renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(context.renderer, &rect);
        SDL_SetRenderDrawColor(context.renderer, 220, 230, 245, 255);
        SDL_RenderDrawRect(context.renderer, &rect);
        label_.SetSize({static_cast<float>(rect.w - 16),
                        static_cast<float>(rect.h - 8)});
        label_.SetAlignment(UIHorizontalAlignment::Center,
                            UIVerticalAlignment::Center);
        label_.Render(context, rect);
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

} // namespace Axion
