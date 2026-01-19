#include "AssetsManager.hpp"

namespace Axion
{

AssetsManager::AssetsManager(EngineContext& ctx) : ContextAware(ctx)
{
    assetsRoot_ = GetPath("assets/");
    INFO("AssetsManager initialized.");
}

AssetsManager::~AssetsManager()
{
    INFO("AssetsManager destroyed.");
}

SDL_Texture* AssetsManager::LoadTexture(const std::string& id, const std::string& relativePath)
{
    auto it = textures_.find(id);
    if (it != textures_.end())
        return it->second;

    const std::string fullPath = assetsRoot_ + relativePath;
    SDL_Surface* surface = IMG_Load(fullPath.c_str());

    if (!surface)
    {
        SDL_Log("IMG_Load failed (%s): %s", fullPath.c_str(), IMG_GetError());
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(ctx_.window->GetRenderer(), surface);
    SDL_FreeSurface(surface);

    if (!texture)
    {
        SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        return nullptr;
    }

    textures_[id] = texture;
    return texture;
}

SDL_Texture* AssetsManager::GetTexture(const std::string& id) const
{
    auto it = textures_.find(id);
    if (it == textures_.end())
        return nullptr;
    return it->second;
}

void AssetsManager::UnloadTexture(const std::string& id)
{
    auto it = textures_.find(id);
    if (it == textures_.end())
        return;

    SDL_DestroyTexture(it->second);
    textures_.erase(it);
}

void AssetsManager::UnloadAllTextures()
{
    for (auto& [id, tex] : textures_)
    {
        SDL_DestroyTexture(tex);
    }
    textures_.clear();
}

} // namespace Axion
