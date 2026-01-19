#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "axion_engine/runtime/components/Component.hpp"
#include "axion_engine/runtime/interfaces/IRenderable.hpp"

namespace Axion
{

/**
 * @brief Base class for components that render visual content.
 * 
 * RenderComponent provides layer and order management for controlling
 * draw order. Derived classes implement the actual rendering logic.
 */
class RenderComponent : public Component, public IRenderable
{
public:
    virtual ~RenderComponent() = default;

    /** @brief Renders this component. Must be implemented by derived classes. */
    virtual void Render(const RenderContext& ctx) = 0;

    /** @brief Sets the render layer. */
    void SetLayer(RenderLayer l) { layer_ = l; }
    
    /** @brief Returns the render layer. */
    RenderLayer GetLayer() const override { return layer_; }

    /** @brief Sets the order within the layer (lower = rendered first). */
    void SetOrderInLayer(int o) { orderInLayer_ = o; }
    
    /** @brief Returns the order within the layer. */
    int GetOrderInLayer() const override { return orderInLayer_; }

protected:
    RenderLayer layer_ = RenderLayer::Default;
    int orderInLayer_ = 0;
};

} // namespace Axion