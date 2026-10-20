#include "CameraComponent.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include "axion_engine/runtime/components/transform/TransformComponent.hpp"

//TODO: Renderers doesn't use the camera's matrixs, must change this to use the camera's view and projection matrixs instead of calculating them again in the renderers. This will allow for more complex camera setups, like multiple cameras, split screen, etc.
namespace Axion
{

glm::mat4 CameraComponent::GetViewMatrix() const
{
    auto* tr = GetOwner()->GetTransform();
    glm::vec3 pos = tr ? tr->GetWorldPosition() : glm::vec3(0.0f);

    glm::vec3 forward(0.0f, 0.0f, -1.0f);
    glm::vec3 target = pos + forward;
    glm::vec3 up(0.0f, 1.0f, 0.0f);

    return glm::lookAt(pos, target, up);
}

} // namespace Axion
