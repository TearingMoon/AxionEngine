#pragma once

#include "ColliderComponent.hpp"
#include <glm/glm.hpp>

class SquareColliderComponent : public ColliderComponent
{
public:
    SquareColliderComponent() = default;

    void Render(const RenderContext &ctx) override {
        
    };

    void SetSize(glm::vec3 size)
    {
        size_ = size;
    }

    glm::vec3 GetSize() const
    {
        return size_ * GetOwner()->GetTransform()->GetScale();
    }

private:
    glm::vec3 size_{1.0f, 1.0f, 1.0f};
};