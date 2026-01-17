#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "axion_engine/runtime/components/Component.hpp"

class TransformComponent : public Component
{
public:
    glm::mat4 GetMatrix() const
    {
        if (dirty_)
        {
            RecalculateMatrix();
        }
        return matrix_;
    }

    glm::vec3 GetPosition() const { return position_; }
    void SetPosition(const glm::vec3 &p)
    {
        position_ = p;
        dirty_ = true;
    }

    glm::vec3 GetRotation() const { return rotationEuler_; }
    void SetRotation(const glm::vec3 &rDeg)
    {
        rotationEuler_ = rDeg;
        dirty_ = true;
    }

    glm::vec3 GetScale() const { return scale_; }
    void SetScale(const glm::vec3 &s)
    {
        scale_ = s;
        dirty_ = true;
    }

    void Translate(float dx, float dy, float dz)
    {
        position_ += glm::vec3(dx, dy, dz);
        dirty_ = true;
    }

    void Rotate(float pitchDeg, float yawDeg, float rollDeg)
    {
        rotationEuler_.x += pitchDeg;
        rotationEuler_.y += yawDeg;
        rotationEuler_.z += rollDeg;
        dirty_ = true;
    }

    glm::vec3 GetForwardVector() const
    {
        glm::mat4 rotationMatrix = GetRotationMatrix();
        glm::vec4 forward = rotationMatrix * glm::vec4(0, 0, -1, 0);
        return glm::normalize(glm::vec3(forward));
    }

    glm::vec3 GetUpVector() const
    {
        glm::mat4 rotationMatrix = GetRotationMatrix();
        glm::vec4 up = rotationMatrix * glm::vec4(0, 1, 0, 0);
        return glm::normalize(glm::vec3(up));
    }

    glm::vec3 GetRightVector() const
    {
        glm::mat4 rotationMatrix = GetRotationMatrix();
        glm::vec4 right = rotationMatrix * glm::vec4(1, 0, 0, 0);
        return glm::normalize(glm::vec3(right));
    }

    glm::mat4 GetRotationMatrix() const
    {
        glm::mat4 r(1.0f);
        r = glm::rotate(r, glm::radians(rotationEuler_.x), glm::vec3(1, 0, 0));
        r = glm::rotate(r, glm::radians(rotationEuler_.y), glm::vec3(0, 1, 0));
        r = glm::rotate(r, glm::radians(rotationEuler_.z), glm::vec3(0, 0, 1));
        return r;
    }

    glm::mat4 GetWorldMatrix() const { return GetMatrix(); }
    glm::vec3 GetWorldPosition() const { return position_; }
    glm::vec3 GetWorldRotation() const { return rotationEuler_; }
    glm::vec3 GetWorldScale() const { return scale_; }
    // TODO: Implement proper world position with parent transforms

private:
    glm::vec3 position_{0.0f};
    glm::vec3 rotationEuler_{0.0f};
    glm::vec3 scale_{1.0f};

    mutable glm::mat4 matrix_{1.0f};
    mutable bool dirty_{true};

    void RecalculateMatrix() const
    {
        glm::mat4 m(1.0f);
        m = glm::translate(m, position_);
        m = glm::rotate(m, glm::radians(rotationEuler_.x), glm::vec3(1, 0, 0));
        m = glm::rotate(m, glm::radians(rotationEuler_.y), glm::vec3(0, 1, 0));
        m = glm::rotate(m, glm::radians(rotationEuler_.z), glm::vec3(0, 0, 1));
        m = glm::scale(m, scale_);
        matrix_ = m;
        dirty_ = false;
    }
};