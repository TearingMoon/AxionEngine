#pragma once
#include "glm/glm.hpp"
#include "axion_engine/runtime/components/Component.hpp"

class CameraComponent : public Component
{
public:
    CameraComponent() = default;

    glm::vec3 WorldToCamera(const glm::vec3 &worldPos) {
        return glm::vec3{};
    };
    glm::vec3 CameraToScreen(const glm::vec3 &cameraPos) {
        return glm::vec3{};
    };

private:
    glm::vec2 viewportSize_{800, 600};
    float fieldOfView_ = 60.0f;
};