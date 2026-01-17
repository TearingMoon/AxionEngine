#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "axion_engine/runtime/components/Component.hpp"
#include "axion_engine/runtime/interfaces/IRenderable.hpp"

class RenderComponent : public Component, public IRenderable
{
public:
    virtual ~RenderComponent() = default;

    virtual void Render(const RenderContext &ctx) = 0;

    void SetLayer(RenderLayer l) { layer_ = l; }
    RenderLayer GetLayer() const override { return layer_; }

    void SetOrderInLayer(int o) { orderInLayer_ = o; }
    int GetOrderInLayer() const override { return orderInLayer_; }

    int GetSortKey() const override { return 0; }

protected:
    RenderLayer layer_ = RenderLayer::Default;
    int orderInLayer_ = 0;
};