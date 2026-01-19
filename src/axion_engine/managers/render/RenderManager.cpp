#include "RenderManager.hpp"

namespace Axion
{

RenderManager::RenderManager(EngineContext& ctx) : ContextAware(ctx)
{
    INFO("RenderManager initialized.");
}

RenderManager::~RenderManager()
{
    INFO("RenderManager destroyed.");
}

void RenderManager::Update()
{
    if (!ctx_.render || !ctx_.scene)
        return;

    renderer_ = ctx_.window->GetRenderer();
    if (!renderer_)
        return;

    Scene* scene = ctx_.sceneProvider->GetCurrentScene();
    if (!scene)
        return;

    CameraComponent* camera = scene->GetCurrentCamera();
    if (!camera)
        return;

    RenderContext rctx;
    rctx.renderer = renderer_;
    rctx.camera = camera;
    SDL_GetRendererOutputSize(renderer_, &rctx.windowWidth, &rctx.windowHeight);

    renderables.clear();
    CollectRenderables(*scene, renderables);

    std::sort(renderables.begin(), renderables.end(),
              [](const IRenderable* a, const IRenderable* b)
              {
                  if (a->GetLayer() != b->GetLayer())
                      return a->GetLayer() < b->GetLayer();
                  return a->GetOrderInLayer() < b->GetOrderInLayer();
              });

    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    if (ctx_.inputProvider->IsKeyJustPressed(SDL_SCANCODE_F3))
    {
        debugMode_ = !debugMode_;
        if (debugMode_)
            INFO("Debug Render Mode Enabled.");
        else
            INFO("Debug Render Mode Disabled.");
    }

    for (IRenderable* r : renderables)
    {
        if (!debugMode_ && r->GetLayer() == RenderLayer::Debug)
            continue;
        r->Render(rctx);
    }
    SDL_RenderPresent(renderer_);
}

void RenderManager::CollectRenderables(Scene& scene, std::vector<IRenderable*>& out)
{
    out.clear();

    for (GameObject* go : scene.GetGameObjects())
    {
        if (!go || !go->IsEnabled() || go->IsDestroyed())
            continue;

        const auto& components = go->GetAllComponents();
        for (const auto& comp : components)
        {
            if (auto* renderable = dynamic_cast<IRenderable*>(comp.get()))
            {
                out.push_back(renderable);
            }
        }
    }
}

} // namespace Axion