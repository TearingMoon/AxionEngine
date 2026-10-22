#include "CameraComponent.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include "axion_engine/runtime/components/transform/TransformComponent.hpp"

namespace Axion
{

glm::mat4 CameraComponent::GetViewMatrix() const
{
    auto* tr = GetOwner()->GetTransform();
    glm::vec3 pos = tr ? tr->GetWorldPosition() : glm::vec3(0.0f);

    const glm::mat4 rotation =
        tr ? tr->GetRotationMatrix() : glm::mat4(1.0f);
    const glm::vec3 forward =
        glm::normalize(glm::vec3(rotation * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
    glm::vec3 target = pos + forward;
    const glm::vec3 up =
        glm::normalize(glm::vec3(rotation * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));

    return glm::lookAt(pos, target, up);
}

glm::vec2 CameraComponent::WorldToScreen(const glm::vec3& worldPosition,
                                         int viewportWidth,
                                         int viewportHeight) const
{
    if (viewportWidth <= 0 || viewportHeight <= 0)
        return {};

    const glm::vec4 clip =
        GetViewProjectionMatrix() * glm::vec4(worldPosition, 1.0f);
    if (clip.w == 0.0f)
        return {};

    const glm::vec3 ndc = glm::vec3(clip) / clip.w;
    return {
        (ndc.x + 1.0f) * 0.5f * static_cast<float>(viewportWidth),
        (1.0f - ndc.y) * 0.5f * static_cast<float>(viewportHeight)};
}

glm::vec2 CameraComponent::WorldSizeToScreen(const glm::vec3& worldPosition,
                                             const glm::vec2& worldSize,
                                             int viewportWidth,
                                             int viewportHeight) const
{
    const glm::vec2 center =
        WorldToScreen(worldPosition, viewportWidth, viewportHeight);
    const glm::vec2 xEdge = WorldToScreen(
        worldPosition + glm::vec3(worldSize.x, 0.0f, 0.0f),
        viewportWidth,
        viewportHeight);
    const glm::vec2 yEdge = WorldToScreen(
        worldPosition + glm::vec3(0.0f, worldSize.y, 0.0f),
        viewportWidth,
        viewportHeight);

    return {
        glm::length(xEdge - center),
        glm::length(yEdge - center)};
}

} // namespace Axion
