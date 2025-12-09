#include "RenderManager.hpp"

RenderManager::RenderManager(EngineContext &ctx) : ContextAware(ctx)
{
    INFO("RenderManager initialized.");
}

RenderManager::~RenderManager()
{
    INFO("RenderManager destroyed.");
}

void RenderManager::Update()
{

    if (!ctx_.render && !ctx_.scene)
        return;

    // Get renderer
    renderer_ = ctx_.window->GetRenderer();
    if (!renderer_)
        return;

    // Get scene
    Scene *scene = ctx_.scene->GetCurrentScene();
    if (!scene)
        return;

    // Get camera
    CameraComponent *camera = scene->GetCurrentCamera();
    if (!camera)
        return;

    // Get Render Items
    std::vector<RenderItem> items;
    CollectRenderItems(items);
    if (items.empty())
    {
        // Clear anyway so you see something
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
        SDL_RenderClear(renderer_);
        SDL_RenderPresent(renderer_);
        return;
    }

    SortRenderItems(items);
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    for (const auto &item : items)
    {
        DrawItem(*camera, item);
    }

    SDL_RenderPresent(renderer_);
}

void RenderManager::CollectRenderItems(std::vector<RenderItem> &outItems)
{
    Scene *scene = ctx_.scene->GetCurrentScene();
    if (!scene)
        return;

    const auto &gameObjects = scene->GetGameObjects();
    outItems.reserve(gameObjects.size());

    for (GameObject *go : gameObjects)
    {
        if (!go || !go->IsEnabled())
            continue;

        if (auto *render = go->GetComponent<RenderComponent>())
        {
            render->CollectRenderItems(outItems);
        }
    }
}

void RenderManager::SortRenderItems(std::vector<RenderItem> &items)
{
    std::sort(items.begin(), items.end(), [](const RenderItem &a, const RenderItem &b)
              {
        if (a.layer != b.layer)
            return a.layer < b.layer;

        if (a.orderInLayer != b.orderInLayer)
            return a.orderInLayer < b.orderInLayer;

        float za = a.worldMatrix[3].z;
        float zb = b.worldMatrix[3].z;
        return za < zb; });
}

void RenderManager::DrawItem(CameraComponent &camera, const RenderItem &item) // TODO; Refactor completely
{
    SpriteRenderComponent *sprite = static_cast<SpriteRenderComponent *>(item.geometry);
    if (!sprite)
        return;

    SDL_Texture *texture = sprite->GetTexture();
    if (!texture)
        return;

    const glm::mat4 &m = item.worldMatrix;

    glm::vec3 worldPos = glm::vec3(m[3]);

    glm::vec3 basisX = glm::vec3(m[0]);
    glm::vec3 basisY = glm::vec3(m[1]);
    float scaleX = glm::length(basisX);
    float scaleY = glm::length(basisY);

    float angleRad = std::atan2(m[0][1], m[0][0]); // θ
    float angleDeg = -glm::degrees(angleRad);

    TransformComponent *camTr = camera.GetOwner()->GetTransform();
    glm::vec3 camPos = camTr ? camTr->GetWorldPosition() : glm::vec3(0.0f);

    int winW = 0, winH = 0;
    SDL_GetRendererOutputSize(renderer_, &winW, &winH);

    glm::vec2 screenPos = WorldToScreen(worldPos, camPos, winW, winH, 1.0f);

    SDL_Rect src;
    if (sprite->HasSourceRect())
    {
        src = sprite->GetSourceRect();
    }
    else
    {
        int texW = 0, texH = 0;
        SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);
        src = {0, 0, texW, texH};
    }

    //TODO: Move this to SpriteRenderComponent
    glm::vec2 sizeWorld = sprite->GetSize() * glm::vec2(sprite->GetOwner()->GetTransform()->GetScale().x, sprite->GetOwner()->GetTransform()->GetScale().y); 
    float dstW = (sizeWorld.x > 0.0f) ? sizeWorld.x : src.w * scaleX;
    float dstH = (sizeWorld.y > 0.0f) ? sizeWorld.y : src.h * scaleY;

    SDL_FRect dst;
    dst.w = dstW;
    dst.h = dstH;

    dst.x = screenPos.x - dst.w * 0.5f;
    dst.y = screenPos.y - dst.h * 0.5f;

    SDL_Color color = sprite->GetColor();
    SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(texture, color.a);

    SDL_RenderCopyExF(
        renderer_,
        texture,
        &src,
        &dst,
        angleDeg,
        nullptr, // center
        SDL_FLIP_NONE);
}

glm::vec2 RenderManager::WorldToScreen(const glm::vec3 &worldPos, const glm::vec3 &camPos, int winW, int winH, float zoom)
{
    float x = (worldPos.x - camPos.x) * zoom + winW * 0.5f;
    // Invert Y axis to make the +Y go up
    float y = winH * 0.5f - (worldPos.y - camPos.y) * zoom;

    return {x, y};
}
