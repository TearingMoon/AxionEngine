#include "OBBColliderComponent.hpp"
#include "CircleColliderComponent.hpp"
#include "AABBColliderComponent.hpp"
#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include "axion_engine/runtime/components/transform/TransformComponent.hpp"

namespace Axion
{

OBBColliderComponent::OBBColliderComponent()
{
    shape_ = ColliderShape::OBB;
}

glm::vec3 OBBColliderComponent::GetSize() const
{
    auto* owner = GetOwner();
    if (!owner) return size_;
    auto* transform = owner->GetTransform();
    if (!transform) return size_;
    return size_ * transform->GetScale();
}

glm::vec2 OBBColliderComponent::GetHalfExtents2D() const
{
    auto* owner = GetOwner();
    if (!owner) return {size_.x * 0.5f, size_.y * 0.5f};
    auto* transform = owner->GetTransform();
    if (!transform) return {size_.x * 0.5f, size_.y * 0.5f};
    const glm::vec3 s = size_ * transform->GetScale();
    return {s.x * 0.5f, s.y * 0.5f};
}

glm::vec2 OBBColliderComponent::GetCenter2D() const
{
    auto* owner = GetOwner();
    if (!owner) return {0.0f, 0.0f};
    auto* transform = owner->GetTransform();
    if (!transform) return {0.0f, 0.0f};
    const glm::vec3 p = transform->GetWorldPosition();
    return {p.x, p.y};
}

void OBBColliderComponent::GetAxes2D(glm::vec2& axisX, glm::vec2& axisY) const
{
    auto* owner = GetOwner();
    auto* tr = owner ? owner->GetTransform() : nullptr;
    const float deg = tr ? tr->GetWorldRotation().z : 0.0f;

    const float rad = DegToRad(deg);
    const float c = std::cos(rad);
    const float s = std::sin(rad);

    axisX = glm::vec2(c, -s);
    axisY = glm::vec2(s, c);

    axisX = glm::normalize(axisX);
    axisY = glm::normalize(axisY);
}

bool OBBColliderComponent::Intersects(const ColliderComponent& other, Manifold& out) const
{
    auto* myOwner = GetOwner();
    if (!myOwner || myOwner->IsDestroyed()) return false;
    return other.IntersectsWithOBB(*this, out);
}

bool OBBColliderComponent::IntersectsWithCircle(const CircleColliderComponent& circle, Manifold& out) const
{
    auto* circleOwner = circle.GetOwner();
    if (!circleOwner || circleOwner->IsDestroyed()) return false;
    auto* circleTransform = circleOwner->GetTransform();
    if (!circleTransform) return false;

    const glm::vec2 cA = GetCenter2D();
    const glm::vec2 hA = GetHalfExtents2D();
    glm::vec2 axA, ayA;
    GetAxes2D(axA, ayA);

    const glm::vec3 cC3 = circleTransform->GetWorldPosition();
    const glm::vec2 cC(cC3.x, cC3.y);
    const float r = circle.GetRadius();

    const glm::vec2 d = cC - cA;

    const float localX = Dot(d, axA);
    const float localY = Dot(d, ayA);

    const float clampedX = Clamp(localX, -hA.x, hA.x);
    const float clampedY = Clamp(localY, -hA.y, hA.y);

    const glm::vec2 closest = cA + axA * clampedX + ayA * clampedY;

    const glm::vec2 diff = cC - closest;
    const float dist2 = Dot(diff, diff);

    if (dist2 > r * r)
    {
        out = {};
        return false;
    }

    const float dist = std::sqrt(dist2);

    out.hit = true;
    if (dist > 0.00001f)
    {
        out.normal = diff / dist;
        out.penetration = r - dist;
    }
    else
    {
        const glm::vec2 dir = cC - cA;
        const float len2 = Dot(dir, dir);
        out.normal = (len2 > 1e-8f) ? (dir / std::sqrt(len2)) : axA;
        out.penetration = r;
    }

    return true;
}

bool OBBColliderComponent::IntersectsWithAABB(const AABBColliderComponent& aabb, Manifold& out) const
{
    const glm::vec3 pB3 = aabb.GetOwner()->GetTransform()->GetWorldPosition();
    const glm::vec3 sB3 = aabb.GetSize();
    const glm::vec2 cB(pB3.x, pB3.y);
    const glm::vec2 hB(sB3.x * 0.5f, sB3.y * 0.5f);
    const glm::vec2 axB(1.0f, 0.0f);
    const glm::vec2 ayB(0.0f, 1.0f);

    const glm::vec2 cA = GetCenter2D();
    const glm::vec2 hA = GetHalfExtents2D();
    glm::vec2 axA, ayA;
    GetAxes2D(axA, ayA);

    out = {};
    const bool hit = SAT_ObbObb(cA, axA, ayA, hA, cB, axB, ayB, hB, out);
    out.hit = hit;
    if (!hit)
        out = {};
    return hit;
}

bool OBBColliderComponent::IntersectsWithOBB(const OBBColliderComponent& other, Manifold& out) const
{
    const glm::vec2 cA = GetCenter2D();
    const glm::vec2 hA = GetHalfExtents2D();
    glm::vec2 axA, ayA;
    GetAxes2D(axA, ayA);

    const glm::vec2 cB = other.GetCenter2D();
    const glm::vec2 hB = other.GetHalfExtents2D();
    glm::vec2 axB, ayB;
    other.GetAxes2D(axB, ayB);

    out = {};
    const bool hit = SAT_ObbObb(cA, axA, ayA, hA, cB, axB, ayB, hB, out);
    out.hit = hit;
    if (!hit)
        out = {};
    return hit;
}

void OBBColliderComponent::Render(const RenderContext& ctx)
{
    auto* tr = GetOwner()->GetTransform();
    if (!tr || !ctx.camera || !ctx.renderer)
        return;

    const glm::vec3 center3 = tr->GetWorldPosition();
    const glm::vec2 center(center3.x, center3.y);

    const glm::vec3 size3 = GetSize();
    const glm::vec2 half(size3.x * 0.5f, size3.y * 0.5f);

    glm::vec2 ax, ay;
    GetAxes2D(ax, ay);

    const glm::vec2 w0 = center + ax * (-half.x) + ay * (-half.y);
    const glm::vec2 w1 = center + ax * (half.x) + ay * (-half.y);
    const glm::vec2 w2 = center + ax * (half.x) + ay * (half.y);
    const glm::vec2 w3 = center + ax * (-half.x) + ay * (half.y);

    const glm::vec3 camWorldPos3 = ctx.camera->GetOwner()->GetTransform()->GetWorldPosition();
    const glm::vec2 camWorldPos(camWorldPos3.x, camWorldPos3.y);

    auto WorldToScreen2D = [&](const glm::vec2& p) -> glm::vec2
    {
        const float x = (p.x - camWorldPos.x) * 1.0f + ctx.windowWidth * 0.5f;
        const float y = ctx.windowHeight * 0.5f - (p.y - camWorldPos.y) * 1.0f;
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
