#include "AABBColliderComponent.hpp"
#include "CircleColliderComponent.hpp"
#include "OBBColliderComponent.hpp"
#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include "axion_engine/runtime/components/transform/TransformComponent.hpp"

glm::vec3 AABBColliderComponent::GetSize() const
{
    return size_ * GetOwner()->GetTransform()->GetScale();
}

void AABBColliderComponent::GetMinMax2D(glm::vec2 &outMin, glm::vec2 &outMax) const
{
    const glm::vec3 p3 = GetOwner()->GetTransform()->GetWorldPosition();
    const glm::vec3 s3 = GetSize();

    const glm::vec2 center(p3.x, p3.y);
    const glm::vec2 half(s3.x * 0.5f, s3.y * 0.5f);

    outMin = center - half;
    outMax = center + half;
}

bool AABBColliderComponent::Intersects(const ColliderComponent &other, Manifold &out) const
{
    return other.IntersectsWithAABB(*this, out);
}

bool AABBColliderComponent::IntersectsWithCircle(const CircleColliderComponent &circle, Manifold &out) const
{
    glm::vec2 aMin, aMax;
    GetMinMax2D(aMin, aMax);

    const glm::vec3 cP3 = circle.GetOwner()->GetTransform()->GetWorldPosition();
    const glm::vec2 c(cP3.x, cP3.y);
    const float r = circle.GetRadius();

    const glm::vec2 closest = ClosestPointOnAABB(c, aMin, aMax);
    const glm::vec2 d = c - closest;
    const float dist2 = glm::dot(d, d);

    if (dist2 > r * r)
    {
        out.hit = false;
        out.penetration = 0.0f;
        out.normal = {0, 0};
        return false;
    }

    const float dist = std::sqrt(dist2);

    out.hit = true;

    if (dist > 0.00001f)
    {
        out.normal = d / dist;
        out.penetration = r - dist;
    }
    else
    {
        const glm::vec2 center = (aMin + aMax) * 0.5f;
        const glm::vec2 half = (aMax - aMin) * 0.5f;
        const glm::vec2 local = c - center;

        const float dx = half.x - std::abs(local.x);
        const float dy = half.y - std::abs(local.y);

        if (dx < dy)
        {
            out.normal = (local.x >= 0.0f) ? glm::vec2(1, 0) : glm::vec2(-1, 0);
            out.penetration = dx + r;
        }
        else
        {
            out.normal = (local.y >= 0.0f) ? glm::vec2(0, 1) : glm::vec2(0, -1);
            out.penetration = dy + r;
        }
    }

    return true;
}

bool AABBColliderComponent::IntersectsWithAABB(const AABBColliderComponent &other, Manifold &out) const
{
    glm::vec2 aMin, aMax, bMin, bMax;
    GetMinMax2D(aMin, aMax);
    other.GetMinMax2D(bMin, bMax);

    const float overlapX = std::min(aMax.x, bMax.x) - std::max(aMin.x, bMin.x);
    const float overlapY = std::min(aMax.y, bMax.y) - std::max(aMin.y, bMin.y);

    if (overlapX <= 0.0f || overlapY <= 0.0f)
    {
        out.hit = false;
        out.penetration = 0.0f;
        out.normal = {0, 0};
        return false;
    }

    const glm::vec3 aP3 = GetOwner()->GetTransform()->GetWorldPosition();
    const glm::vec3 bP3 = other.GetOwner()->GetTransform()->GetWorldPosition();
    const glm::vec2 delta(bP3.x - aP3.x, bP3.y - aP3.y);

    out.hit = true;

    if (overlapX < overlapY)
    {
        out.penetration = overlapX;
        out.normal = (delta.x >= 0.0f) ? glm::vec2(1, 0) : glm::vec2(-1, 0);
    }
    else
    {
        out.penetration = overlapY;
        out.normal = (delta.y >= 0.0f) ? glm::vec2(0, 1) : glm::vec2(0, -1);
    }

    return true;
}

bool AABBColliderComponent::IntersectsWithOBB(const OBBColliderComponent &other, Manifold &out) const
{
    return other.IntersectsWithAABB(*this, out);
}

void AABBColliderComponent::Render(const RenderContext &ctx)
{
    // TODO: Implement AABB rendering
}
