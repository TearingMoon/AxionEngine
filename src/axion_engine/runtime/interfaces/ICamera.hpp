#pragma once

#include <glm/glm.hpp>

#include "axion_engine/runtime/components/Component.hpp"

enum class ProjectionType
{
    Perspective,
    Orthographic
};

class ICamera : public Component
{
public:
    virtual ~ICamera() = default;

    virtual glm::mat4 GetViewMatrix() const = 0;
    virtual glm::mat4 GetProjectionMatrix() const = 0;

    glm::mat4 GetViewProjectionMatrix() const
    {
        return GetProjectionMatrix() * GetViewMatrix();
    }

    void SetProjectionType(ProjectionType type) { projectionType_ = type; }
    ProjectionType GetProjectionType() const { return projectionType_; }

protected:
    ProjectionType projectionType_ = ProjectionType::Orthographic;
};