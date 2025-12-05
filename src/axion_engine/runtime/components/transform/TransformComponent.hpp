#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "axion_engine/runtime/components/Component.hpp"
#include "axion_engine/runtime/interfaces/IUpdateable.hpp"

class TransformComponent : public Component
{
public:
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 rotationEuler{0.0f, 0.0f, 0.0f}; // degrees
    glm::vec3 scale{1.0f, 1.0f, 1.0f};

    glm::mat4 GetMatrix() const
    {
        glm::mat4 m(1.0f);
        m = glm::translate(m, position);
        m = glm::rotate(m, glm::radians(rotationEuler.x), glm::vec3(1, 0, 0));
        m = glm::rotate(m, glm::radians(rotationEuler.y), glm::vec3(0, 1, 0));
        m = glm::rotate(m, glm::radians(rotationEuler.z), glm::vec3(0, 0, 1));
        m = glm::scale(m, scale);
        return m;
    }

    void Rotate(float pitch, float yaw, float roll)
    {
        rotationEuler.x += pitch;
        rotationEuler.y += yaw;
        rotationEuler.z += roll;
    }

    glm::mat4 GetWorldMatrix() const { return GetMatrix(); }
    glm::vec3 GetWorldPosition() const { return position; }
};