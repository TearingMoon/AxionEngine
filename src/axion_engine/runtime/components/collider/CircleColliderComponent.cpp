#include "CircleColliderComponent.hpp"
#include "AABBColliderComponent.hpp"
#include "OBBColliderComponent.hpp"
#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include "axion_engine/runtime/components/transform/TransformComponent.hpp"

float CircleColliderComponent::GetRadius() const
{
    const glm::vec3 s = GetOwner()->GetTransform()->GetScale();
    const float uniform = std::max(s.x, s.y);
    return radius_ * uniform;
}

bool CircleColliderComponent::Intersects(const ColliderComponent &other, Manifold &out) const
{
    return other.IntersectsWithCircle(*this, out);
}

bool CircleColliderComponent::IntersectsWithCircle(const CircleColliderComponent &other, Manifold &out) const
{
    const glm::vec3 a3 = GetOwner()->GetTransform()->GetWorldPosition();
    const glm::vec3 b3 = other.GetOwner()->GetTransform()->GetWorldPosition();

    const glm::vec2 a(a3.x, a3.y);
    const glm::vec2 b(b3.x, b3.y);

    const float ra = GetRadius();
    const float rb = other.GetRadius();

    const glm::vec2 d = b - a;
    const float dist2 = glm::dot(d, d);
    const float r = ra + rb;
    const float r2 = r * r;

    if (dist2 > r2)
    {
        out.hit = false;
        out.penetration = 0.0f;
        out.normal = {0, 0};
        return false;
    }

    const float dist = std::sqrt(dist2);

    out.hit = true;
    out.penetration = (dist > 0.00001f) ? (r - dist) : r;
    out.normal = (dist > 0.00001f) ? (d / dist) : glm::vec2(1.0f, 0.0f);
    return true;
}

bool CircleColliderComponent::IntersectsWithAABB(const AABBColliderComponent &other, Manifold &out) const
{
    return other.IntersectsWithCircle(*this, out);
}

bool CircleColliderComponent::IntersectsWithOBB(const OBBColliderComponent &other, Manifold &out) const
{
    return other.IntersectsWithCircle(*this, out);
}

void CircleColliderComponent::Render(const RenderContext &ctx)
{
    auto *transform = GetOwner()->GetTransform();
    if (!transform || !ctx.camera || !ctx.renderer)
        return;

    const glm::vec3 worldPos = transform->GetWorldPosition();
    const glm::vec3 camWorldPos = ctx.camera->GetOwner()->GetTransform()->GetWorldPosition();

    auto WorldToScreen2D = [&](const glm::vec3 &p) -> glm::vec2
    {
        const float x = (p.x - camWorldPos.x) * 1.0f + ctx.windowWidth * 0.5f;
        const float y = ctx.windowHeight * 0.5f - (p.y - camWorldPos.y) * 1.0f;
        return {x, y};
    };

    const glm::vec2 screenPos = WorldToScreen2D(worldPos);

    const float worldRadius = GetRadius();
    const glm::vec3 radiusWorldPoint = worldPos + glm::vec3(worldRadius, 0.0f, 0.0f);

    const glm::vec2 radiusScreenPoint = WorldToScreen2D(radiusWorldPoint);

    int screenRadius = (int)glm::abs(radiusScreenPoint.x - screenPos.x);
    if (screenRadius < 1)
        screenRadius = 1;

    if (IsTrigger())
        SDL_SetRenderDrawColor(ctx.renderer, 0, 255, 255, 255);
    else
        SDL_SetRenderDrawColor(ctx.renderer, 0, 255, 0, 255);

    DrawCircle(ctx, (int)screenPos.x, (int)screenPos.y, screenRadius);

    SDL_SetRenderDrawColor(ctx.renderer, 255, 0, 0, 255);
    SDL_RenderDrawPoint(ctx.renderer, (int)screenPos.x, (int)screenPos.y);
}

void CircleColliderComponent::DrawCircle(const RenderContext &ctx, int centerX, int centerY, int radius)
{
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    auto drawCirclePoints = [&](int cx, int cy, int x, int y)
    {
        SDL_RenderDrawPoint(ctx.renderer, cx + x, cy + y);
        SDL_RenderDrawPoint(ctx.renderer, cx - x, cy + y);
        SDL_RenderDrawPoint(ctx.renderer, cx + x, cy - y);
        SDL_RenderDrawPoint(ctx.renderer, cx - x, cy - y);
        SDL_RenderDrawPoint(ctx.renderer, cx + y, cy + x);
        SDL_RenderDrawPoint(ctx.renderer, cx - y, cy + x);
        SDL_RenderDrawPoint(ctx.renderer, cx + y, cy - x);
        SDL_RenderDrawPoint(ctx.renderer, cx - y, cy - x);
    };

    while (y >= x)
    {
        drawCirclePoints(centerX, centerY, x, y);
        x++;

        if (d > 0)
        {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        else
        {
            d = d + 4 * x + 6;
        }
    }
}
