#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "axion_engine/runtime/components/Component.hpp"

namespace Axion
{

/**
 * @brief Component that stores position, rotation, and scale of a GameObject.
 * 
 * TransformComponent provides spatial information and transformation matrices
 * for positioning objects in the game world. Uses lazy evaluation for matrix
 * calculation to avoid unnecessary recomputation.
 */
class TransformComponent : public Component
{
public:
    /**
     * @brief Returns the local transformation matrix.
     * @return Combined translation, rotation, and scale matrix
     */
    glm::mat4 GetMatrix() const
    {
        if (dirty_)
            RecalculateMatrix();
        return matrix_;
    }

    /** @brief Returns the local position. */
    glm::vec3 GetPosition() const { return position_; }
    
    /**
     * @brief Sets the local position.
     * @param p New position vector
     */
    void SetPosition(const glm::vec3& p)
    {
        position_ = p;
        dirty_ = true;
    }

    /** @brief Returns the local rotation in degrees (Euler angles). */
    glm::vec3 GetRotation() const { return rotationEuler_; }
    
    /**
     * @brief Sets the local rotation.
     * @param rDeg Rotation in degrees (pitch, yaw, roll)
     */
    void SetRotation(const glm::vec3& rDeg)
    {
        rotationEuler_ = rDeg;
        dirty_ = true;
    }

    /** @brief Returns the local scale. */
    glm::vec3 GetScale() const { return scale_; }
    
    /**
     * @brief Sets the local scale.
     * @param s Scale vector
     */
    void SetScale(const glm::vec3& s)
    {
        scale_ = s;
        dirty_ = true;
    }

    /**
     * @brief Moves the transform by the given offset.
     * @param dx X offset
     * @param dy Y offset  
     * @param dz Z offset
     */
    void Translate(float dx, float dy, float dz)
    {
        position_ += glm::vec3(dx, dy, dz);
        dirty_ = true;
    }

    /**
     * @brief Rotates the transform by the given angles.
     * @param pitchDeg Rotation around X axis in degrees
     * @param yawDeg Rotation around Y axis in degrees
     * @param rollDeg Rotation around Z axis in degrees
     */
    void Rotate(float pitchDeg, float yawDeg, float rollDeg)
    {
        rotationEuler_.x += pitchDeg;
        rotationEuler_.y += yawDeg;
        rotationEuler_.z += rollDeg;
        dirty_ = true;
    }

    /** @brief Returns the forward direction vector (negative Z). */
    glm::vec3 GetForwardVector() const
    {
        glm::mat4 rotationMatrix = GetRotationMatrix();
        glm::vec4 forward = rotationMatrix * glm::vec4(0, 0, -1, 0);
        return glm::normalize(glm::vec3(forward));
    }

    /** @brief Returns the up direction vector (positive Y). */
    glm::vec3 GetUpVector() const
    {
        glm::mat4 rotationMatrix = GetRotationMatrix();
        glm::vec4 up = rotationMatrix * glm::vec4(0, 1, 0, 0);
        return glm::normalize(glm::vec3(up));
    }

    /** @brief Returns the right direction vector (positive X). */
    glm::vec3 GetRightVector() const
    {
        glm::mat4 rotationMatrix = GetRotationMatrix();
        glm::vec4 right = rotationMatrix * glm::vec4(1, 0, 0, 0);
        return glm::normalize(glm::vec3(right));
    }

    /** @brief Returns the rotation-only matrix. */
    glm::mat4 GetRotationMatrix() const
    {
        glm::mat4 r(1.0f);
        r = glm::rotate(r, glm::radians(rotationEuler_.x), glm::vec3(1, 0, 0));
        r = glm::rotate(r, glm::radians(rotationEuler_.y), glm::vec3(0, 1, 0));
        r = glm::rotate(r, glm::radians(rotationEuler_.z), glm::vec3(0, 0, 1));
        return r;
    }

    /** @brief Returns the world transformation matrix. */
    glm::mat4 GetWorldMatrix() const { return GetMatrix(); }
    
    /** @brief Returns the world position. */
    glm::vec3 GetWorldPosition() const { return position_; }
    
    /** @brief Returns the world rotation. */
    glm::vec3 GetWorldRotation() const { return rotationEuler_; }
    
    /** @brief Returns the world scale. */
    glm::vec3 GetWorldScale() const { return scale_; }

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

} // namespace Axion