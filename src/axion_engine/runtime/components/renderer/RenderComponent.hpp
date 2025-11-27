#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "axion_engine/runtime/components/Component.hpp"

struct RenderItem
{
    glm::mat4 worldMatrix;
    void *geometry = nullptr;
    void *material = nullptr;
    int layer = 0;
    int orderInLayer = 0;
    bool transparent = false;
};

class RenderComponent : public Component
{
public:
    virtual ~RenderComponent() = default;
    virtual void CollectRenderItems(std::vector<RenderItem> &outItems) = 0;

    void SetLayer(int l) { layer_ = l; }
    int GetLayer() const { return layer_; }

    void SetOrderInLayer(int o) { orderInLayer_ = o; }
    int GetOrderInLayer() const { return orderInLayer_; }

protected:
    int layer_ = 0;
    int orderInLayer_ = 0;
};