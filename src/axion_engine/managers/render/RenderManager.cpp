#include "RenderManager.hpp"

#include <algorithm>
#include <array>
#include <cstdio>

#include "axion_engine/managers/analysis/Analyzer.hpp"

namespace Axion
{

RenderManager::RenderManager(EngineContext& ctx) : ContextAware(ctx)
{
    AXION_INFO("RenderManager initialized.");
}

RenderManager::~RenderManager()
{
    DestroyProfilerOverlay();
    AXION_INFO("RenderManager destroyed.");
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
            AXION_INFO("Debug Render Mode Enabled.");
        else
            AXION_INFO("Debug Render Mode Disabled.");
    }

    if (ctx_.inputProvider->IsKeyJustPressed(SDL_SCANCODE_F4))
    {
        profilerMode_ = !profilerMode_;
        if (!profilerMode_)
            DestroyProfilerOverlay();

        AXION_INFO(profilerMode_
                       ? "Profiler Overlay Enabled."
                       : "Profiler Overlay Disabled.");
    }

    for (IRenderable* r : renderables)
    {
        if (!debugMode_ && r->GetLayer() == RenderLayer::Debug)
            continue;
        r->Render(rctx);
    }

    if (profilerMode_)
        RenderProfilerOverlay();

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

void RenderManager::RenderProfilerOverlay()
{
    if (!ctx_.analyzer)
        return;

    int windowWidth = 0;
    int windowHeight = 0;
    SDL_GetRendererOutputSize(renderer_, &windowWidth, &windowHeight);

    const int fontSize = std::clamp(windowHeight / 42, 12, 18);
    if (!LoadProfilerFont(fontSize))
        return;

    profilerRefreshTimer_ += ctx_.timeProvider
                                 ? ctx_.timeProvider->GetDeltaTime()
                                 : 0.0f;
    if (profilerLines_.empty() || profilerRefreshTimer_ >= 0.1f)
    {
        profilerRefreshTimer_ = 0.0f;
        UpdateProfilerTexture();
    }

    if (profilerLines_.empty())
        return;

    constexpr int padding = 14;
    constexpr int lineGap = 3;
    int panelWidth = std::min(windowWidth - 16, 480);
    int panelHeight = padding * 2;
    for (const auto& line : profilerLines_)
    {
        const int availableWidth = panelWidth - padding * 2;
        const float scale = line.width > availableWidth
                                ? static_cast<float>(availableWidth) / line.width
                                : 1.0f;
        panelHeight += static_cast<int>(line.height * scale) + lineGap;
    }
    panelHeight -= lineGap;

    if (panelWidth < 160 || panelHeight > windowHeight - 16)
        return;

    SDL_Rect background{
        8,
        8,
        panelWidth,
        panelHeight};

    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer_, 10, 15, 25, 235);
    SDL_RenderFillRect(renderer_, &background);

    SDL_SetRenderDrawColor(renderer_, 65, 150, 220, 255);
    SDL_Rect topBorder{background.x, background.y, background.w, 3};
    SDL_RenderFillRect(renderer_, &topBorder);

    int y = background.y + padding;
    for (const auto& line : profilerLines_)
    {
        const int availableWidth = panelWidth - padding * 2;
        const float scale = line.width > availableWidth
                                ? static_cast<float>(availableWidth) / line.width
                                : 1.0f;
        SDL_Rect destination{
            background.x + padding,
            y,
            static_cast<int>(line.width * scale),
            static_cast<int>(line.height * scale)};
        SDL_RenderCopy(renderer_, line.texture, nullptr, &destination);
        y += destination.h + lineGap;
    }
}

bool RenderManager::LoadProfilerFont(int fontSize)
{
    if (profilerFont_ && profilerFontSize_ == fontSize)
        return true;

    if (profilerFont_)
    {
        TTF_CloseFont(profilerFont_);
        profilerFont_ = nullptr;
    }

    const std::array<const char*, 8> fontPaths = {
        "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationMono-Regular.ttf",
        "/usr/share/fonts/truetype/freefont/FreeMono.ttf",
        "/System/Library/Fonts/Menlo.ttc",
        "/Library/Fonts/Arial.ttf"};

    for (const char* path : fontPaths)
    {
        profilerFont_ = TTF_OpenFont(path, fontSize);
        if (profilerFont_)
        {
            profilerFontSize_ = fontSize;
            profilerText_.clear();
            return true;
        }
    }

    AXION_WARN("Profiler overlay could not load a font: %s", TTF_GetError());
    return false;
}

void RenderManager::UpdateProfilerTexture()
{
    const auto& metrics = ctx_.analyzer->GetMetrics();
    const auto& sections = ctx_.analyzer->GetAllSectionMetrics();

    auto sectionTime = [&sections](const char* name)
    {
        const auto it = sections.find(name);
        return it != sections.end() ? it->second.lastMs : 0.0f;
    };

    char buffer[1024];
    std::snprintf(buffer, sizeof(buffer),
                  "%.2f|%.1f|%.2f|%.2f|%.2f|%.2f|%.2f|%.2f|%zu|%zu|%zu",
                  metrics.frameTime, metrics.fps, metrics.avgFrameTime,
                  sectionTime("Time"), sectionTime("SceneRequests"),
                  sectionTime("Physics"), sectionTime("SceneUpdate"),
                  sectionTime("Render"), metrics.gameObjectCount,
                  metrics.componentCount, metrics.colliderCount);

    const std::string text(buffer);
    if (text == profilerText_)
        return;

    for (auto& line : profilerLines_)
        SDL_DestroyTexture(line.texture);
    profilerLines_.clear();

    char lineText[128];
    const std::array<std::pair<const char*, SDL_Color>, 7> labels = {{
        {"AXION PROFILER  [F4]", {120, 205, 255, 255}},
        {"Frame     %6.2f ms    FPS       %6.1f", {235, 240, 250, 255}},
        {"Average   %6.2f ms", {180, 195, 215, 255}},
        {"Time      %6.2f ms    Requests  %6.2f ms", {180, 195, 215, 255}},
        {"Physics   %6.2f ms    Scene     %6.2f ms", {255, 205, 125, 255}},
        {"Render    %6.2f ms", {180, 195, 215, 255}},
        {"Objects   %zu       Components %zu       Colliders %zu", {135, 235, 180, 255}}
    }};

    for (size_t i = 0; i < labels.size(); ++i)
    {
        if (i == 0)
            std::snprintf(lineText, sizeof(lineText), "%s", labels[i].first);
        else if (i == 1)
            std::snprintf(lineText, sizeof(lineText), labels[i].first,
                          metrics.frameTime, metrics.fps);
        else if (i == 2)
            std::snprintf(lineText, sizeof(lineText), labels[i].first,
                          metrics.avgFrameTime);
        else if (i == 3)
            std::snprintf(lineText, sizeof(lineText), labels[i].first,
                          sectionTime("Time"), sectionTime("SceneRequests"));
        else if (i == 4)
            std::snprintf(lineText, sizeof(lineText), labels[i].first,
                          sectionTime("Physics"), sectionTime("SceneUpdate"));
        else if (i == 5)
            std::snprintf(lineText, sizeof(lineText), labels[i].first,
                          sectionTime("Render"));
        else
            std::snprintf(lineText, sizeof(lineText), labels[i].first,
                          metrics.gameObjectCount, metrics.componentCount,
                          metrics.colliderCount);

        SDL_Surface* surface =
            TTF_RenderUTF8_Blended(profilerFont_, lineText, labels[i].second);
        if (!surface)
        {
            AXION_WARN("Profiler overlay text rendering failed: %s", TTF_GetError());
            continue;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
        const int width = surface->w;
        const int height = surface->h;
        SDL_FreeSurface(surface);
        if (!texture)
        {
            AXION_WARN("Profiler overlay texture creation failed: %s", SDL_GetError());
            continue;
        }

        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        profilerLines_.push_back({texture, width, height, labels[i].second});
    }

    profilerText_ = text;
}

void RenderManager::DestroyProfilerOverlay()
{
    for (auto& line : profilerLines_)
    {
        if (line.texture)
            SDL_DestroyTexture(line.texture);
    }
    profilerLines_.clear();

    if (profilerFont_)
    {
        TTF_CloseFont(profilerFont_);
        profilerFont_ = nullptr;
    }

    profilerFontSize_ = 0;
    profilerText_.clear();
    profilerRefreshTimer_ = 0.0f;
}

} // namespace Axion