#pragma once

#include <SDL.h>

#include "RenderComponent.hpp"
#include "axion_engine/runtime/components/transform/TransformComponent.hpp"

class SpriteRenderComponent : public RenderComponent
{
public:
    void SetTexture(SDL_Texture *tex) { texture_ = tex; }
    SDL_Texture *GetTexture() const { return texture_; }

    void SetSourceRect(const SDL_Rect &r)
    {
        srcRect_ = r;
        hasSrcRect_ = true;
    }

    void SetColor(const SDL_Color &col) { color_ = col; }

    void SetSize(const glm::vec2 &size) { size_ = size; }

    virtual void CollectRenderItems(std::vector<RenderItem> &outItems) override
    {
        if (!texture_)
            return;

        auto *tr = GetOwner()->GetComponent<TransformComponent>();
        if (!tr)
            return;

        RenderItem item;
        item.worldMatrix = tr->GetWorldMatrix();
        item.geometry = this;
        item.layer = GetLayer();
        item.orderInLayer = GetOrderInLayer();
        outItems.push_back(item);
    }

    bool HasSourceRect() const
    {
        return hasSrcRect_;
    }

    const SDL_Rect &GetSourceRect() const
    {
        return srcRect_;
    }

    glm::vec2 GetSize() const
    {
        return size_;
    }

    SDL_Color GetColor() const
    {
        return color_;
    }

private:
    SDL_Texture *texture_ = nullptr;
    SDL_Rect srcRect_{0, 0, 0, 0};
    bool hasSrcRect_ = false;
    SDL_Color color_{255, 255, 255, 255};
    glm::vec2 size_{1.0f, 1.0f};
};
