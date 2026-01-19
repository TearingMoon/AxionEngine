#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "axion_engine/runtime/components/Component.hpp"

namespace Axion
{

class TransformComponent;

/**
 * @brief Camera projection type.
 */
enum class ProjectionType
{
    Perspective,    ///< 3D perspective projection
    Orthographic    ///< 2D orthographic projection
};

/**
 * @brief Component that defines a camera for rendering.
 * 
 * CameraComponent provides view and projection matrices for rendering.
 * Supports both perspective and orthographic projections.
 */
class CameraComponent : public Component
{
public:
    void SetProjectionType(ProjectionType t) { projectionType_ = t; }
    ProjectionType GetProjectionType() const { return projectionType_; }

    /**
     * @brief Configures perspective projection.
     * @param fovDegrees Vertical field of view in degrees
     * @param aspect Aspect ratio (width/height)
     * @param nearPlane Near clipping plane distance
     * @param farPlane Far clipping plane distance
     */
    void SetPerspective(float fovDegrees, float aspect, float nearPlane, float farPlane)
    {
        projectionType_ = ProjectionType::Perspective;
        fovRadians_ = glm::radians(fovDegrees);
        aspect_ = aspect;
        nearPlane_ = nearPlane;
        farPlane_ = farPlane;
    }

    /**
     * @brief Configures orthographic projection.
     * @param width Viewport width in world units
     * @param height Viewport height in world units
     * @param nearPlane Near clipping plane
     * @param farPlane Far clipping plane
     */
    void SetOrthographic(float width, float height, float nearPlane, float farPlane)
    {
        projectionType_ = ProjectionType::Orthographic;
        orthoWidth_ = width;
        orthoHeight_ = height;
        nearPlane_ = nearPlane;
        farPlane_ = farPlane;
    }

    /** @brief Returns the view matrix based on camera position. */
    glm::mat4 GetViewMatrix() const;

    /** @brief Returns the projection matrix. */
    glm::mat4 GetProjectionMatrix() const
    {
        if (projectionType_ == ProjectionType::Perspective)
        {
            return glm::perspective(fovRadians_, aspect_, nearPlane_, farPlane_);
        }
        else
        {
            float hw = orthoWidth_ * 0.5f;
            float hh = orthoHeight_ * 0.5f;
            return glm::ortho(-hw, hw, -hh, hh, nearPlane_, farPlane_);
        }
    }

    /** @brief Returns the combined view-projection matrix. */
    glm::mat4 GetViewProjectionMatrix() const
    {
        return GetProjectionMatrix() * GetViewMatrix();
    }

private:
    ProjectionType projectionType_ = ProjectionType::Orthographic;

    float nearPlane_ = -1.0f;
    float farPlane_ = 1.0f;

    float fovRadians_ = glm::radians(60.0f);
    float aspect_ = 16.0f / 9.0f;

    float orthoWidth_ = 10.0f;
    float orthoHeight_ = 10.0f;
};

} // namespace Axion