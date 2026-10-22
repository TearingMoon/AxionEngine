#pragma once

#include <memory>
#include <vector>

#include "UIElement.hpp"
#include "UILabel.hpp"
#include "UIPanel.hpp"
#include "UIButton.hpp"
#include "axion_engine/runtime/components/renderer/RenderComponent.hpp"
#include "axion_engine/runtime/interfaces/IUpdateable.hpp"

namespace Axion
{

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
        if (!context.window || !context.inputProvider)
            return;
        int width = 0;
        int height = 0;
        SDL_GetRendererOutputSize(context.window->GetRenderer(), &width, &height);
        UIRenderContext ui{context.window->GetRenderer(), context.assets,
                           context.inputProvider, width, height};
        for (auto& child : children_)
            child->Update(ui, {0, 0, width, height});
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
            child->Render(ui, {0, 0, context.windowWidth, context.windowHeight});
    }

private:
    std::vector<std::unique_ptr<UIElement>> children_;
};

} // namespace Axion
