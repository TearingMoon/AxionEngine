#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "axion_engine/runtime/components/Component.hpp"

class TransformComponent;

enum class ProjectionType
{
    Perspective,
    Orthographic
};

class CameraComponent : public Component
{
public:
    void SetProjectionType(ProjectionType t) { projectionType_ = t; }
    ProjectionType GetProjectionType() const { return projectionType_; }

    void SetPerspective(float fovDegrees, float aspect, float nearPlane, float farPlane)
    {
        projectionType_ = ProjectionType::Perspective;
        fovRadians_ = glm::radians(fovDegrees);
        aspect_ = aspect;
        nearPlane_ = nearPlane;
        farPlane_ = farPlane;
    }

    void SetOrthographic(float width, float height, float nearPlane, float farPlane)
    {
        projectionType_ = ProjectionType::Orthographic;
        orthoWidth_ = width;
        orthoHeight_ = height;
        nearPlane_ = nearPlane;
        farPlane_ = farPlane;
    }

    glm::mat4 GetViewMatrix() const;

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