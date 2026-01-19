#include "AABBColliderComponent.hpp"
#include "CircleColliderComponent.hpp"
#include "OBBColliderComponent.hpp"
#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include "axion_engine/runtime/components/transform/TransformComponent.hpp"

namespace Axion
{

AABBColliderComponent::AABBColliderComponent()
{
    shape_ = ColliderShape::AABB;
}

glm::vec3 AABBColliderComponent::GetSize() const
{
    auto* owner = GetOwner();
    if (!owner) return size_;
    auto* transform = owner->GetTransform();
    if (!transform) return size_;
    return size_ * transform->GetScale();
}

void AABBColliderComponent::GetMinMax2D(glm::vec2& outMin, glm::vec2& outMax) const
{
    auto* owner = GetOwner();
    if (!owner)
    {
        outMin = outMax = glm::vec2(0);
        return;
    }

    auto* transform = owner->GetTransform();
    if (!transform)
    {
        outMin = outMax = glm::vec2(0);
        return;
    }

    const glm::vec3 p3 = transform->GetWorldPosition();
    const glm::vec3 s3 = GetSize();

    const glm::vec2 center(p3.x, p3.y);
    const glm::vec2 half(s3.x * 0.5f, s3.y * 0.5f);

    outMin = center - half;
    outMax = center + half;
}

bool AABBColliderComponent::Intersects(const ColliderComponent& other, Manifold& out) const
{
    auto* myOwner = GetOwner();
    if (!myOwner || myOwner->IsDestroyed()) return false;
    return other.IntersectsWithAABB(*this, out);
}

bool AABBColliderComponent::IntersectsWithCircle(const CircleColliderComponent& circle, Manifold& out) const
{
    auto* circleOwner = circle.GetOwner();
    if (!circleOwner || circleOwner->IsDestroyed()) return false;
    auto* circleTransform = circleOwner->GetTransform();
    if (!circleTransform) return false;

    glm::vec2 aMin, aMax;
    GetMinMax2D(aMin, aMax);

    const glm::vec3 cP3 = circleTransform->GetWorldPosition();
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

bool AABBColliderComponent::IntersectsWithAABB(const AABBColliderComponent& other, Manifold& out) const
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

bool AABBColliderComponent::IntersectsWithOBB(const OBBColliderComponent& other, Manifold& out) const
{
    return other.IntersectsWithAABB(*this, out);
}

void AABBColliderComponent::Render(const RenderContext& ctx)
{
    auto* tr = GetOwner()->GetTransform();
    if (!tr || !ctx.camera || !ctx.renderer)
        return;

    const glm::vec3 center3 = tr->GetWorldPosition();
    const glm::vec2 center(center3.x, center3.y);

    const glm::vec3 size3 = GetSize();
    const glm::vec2 half(size3.x * 0.5f, size3.y * 0.5f);

    const glm::vec2 w0(center.x - half.x, center.y - half.y);
    const glm::vec2 w1(center.x + half.x, center.y - half.y);
    const glm::vec2 w2(center.x + half.x, center.y + half.y);
    const glm::vec2 w3(center.x - half.x, center.y + half.y);

    const glm::vec3 camWorldPos3 = ctx.camera->GetOwner()->GetTransform()->GetWorldPosition();
    const glm::vec2 camWorldPos(camWorldPos3.x, camWorldPos3.y);

    auto WorldToScreen2D = [&](const glm::vec2& p) -> glm::vec2
    {
        const float x = (p.x - camWorldPos.x) * 1.0f + ctx.windowWidth * 0.5f;
        const float y = (p.y - camWorldPos.y) * 1.0f + ctx.windowHeight * 0.5f;
        return {x, y};
    };

    const glm::vec2 s0 = WorldToScreen2D(w0);
    const glm::vec2 s1 = WorldToScreen2D(w1);
    const glm::vec2 s2 = WorldToScreen2D(w2);
    const glm::vec2 s3 = WorldToScreen2D(w3);

    if (IsTrigger())
        SDL_SetRenderDrawColor(ctx.renderer, 0, 255, 255, 255);
    else
        SDL_SetRenderDrawColor(ctx.renderer, 0, 255, 0, 255);

    SDL_RenderDrawLine(ctx.renderer, (int)s0.x, (int)s0.y, (int)s1.x, (int)s1.y);
    SDL_RenderDrawLine(ctx.renderer, (int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y);
    SDL_RenderDrawLine(ctx.renderer, (int)s2.x, (int)s2.y, (int)s3.x, (int)s3.y);
    SDL_RenderDrawLine(ctx.renderer, (int)s3.x, (int)s3.y, (int)s0.x, (int)s0.y);

    SDL_SetRenderDrawColor(ctx.renderer, 255, 0, 0, 255);
    const glm::vec2 sc = WorldToScreen2D(center);
    SDL_RenderDrawPoint(ctx.renderer, (int)sc.x, (int)sc.y);
}

} // namespace Axion
