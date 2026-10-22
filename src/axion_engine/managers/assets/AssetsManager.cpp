#include "AssetsManager.hpp"
#include <algorithm>
#include <filesystem>

namespace Axion
{

    AssetsManager::AssetsManager(EngineContext &ctx) : ContextAware(ctx)
    {
        assetsRoot_ = GetPath("assets/");
        AXION_INFO("AssetsManager initialized.");
    }

    AssetsManager::~AssetsManager()
    {
        UnloadAllTextures();
        for (auto &[id, font] : fonts_)
            TTF_CloseFont(font);
        fonts_.clear();
        AXION_INFO("AssetsManager destroyed.");
    }

    SDL_Texture *AssetsManager::LoadTexture(const std::string &id, const std::string &relativePath)
    {
        auto it = textures_.find(id);
        if (it != textures_.end())
            return it->second;

        const std::string fullPath = assetsRoot_ + relativePath;
        SDL_Surface *surface = IMG_Load(fullPath.c_str());

        if (!surface)
        {
            SDL_Log("IMG_Load failed (%s): %s", fullPath.c_str(), IMG_GetError());
            return nullptr;
        }

        SDL_Texture *texture = SDL_CreateTextureFromSurface(ctx_.window->GetRenderer(), surface);
        SDL_FreeSurface(surface);

        if (!texture)
        {
            SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
            return nullptr;
        }

        textures_[id] = texture;
        return texture;
    }

    SDL_Texture *AssetsManager::GetTexture(const std::string &id) const
    {
        auto it = textures_.find(id);
        if (it == textures_.end())
            return nullptr;
        return it->second;
    }

    TTF_Font* AssetsManager::LoadFont(const std::string& id,
                                       const std::string& relativePath,
                                       int pointSize)
    {
        const std::filesystem::path path(relativePath);
        if (id.empty() || relativePath.empty() || pointSize <= 0 ||
            path.is_absolute() ||
            std::find(path.begin(), path.end(), "..") != path.end())
        {
            AXION_WARN("Rejected invalid font asset request '{}'.", relativePath);
            return nullptr;
        }

        const std::string cacheKey =
            id + "@" + std::to_string(pointSize);
        auto it = fonts_.find(cacheKey);
        if (it != fonts_.end())
            return it->second;
        if (failedFonts_.find(cacheKey) != failedFonts_.end())
            return nullptr;

        const std::string fullPath = assetsRoot_ + relativePath;
        TTF_Font* font = TTF_OpenFont(fullPath.c_str(), pointSize);
        if (!font)
        {
            AXION_WARN("Font '{}' could not be loaded from '{}': {}",
                       id, fullPath, TTF_GetError());
            failedFonts_.insert(cacheKey);
            return nullptr;
        }

        fonts_[cacheKey] = font;
        return font;
    }

    TTF_Font* AssetsManager::GetFont(const std::string& id, int pointSize) const
    {
        const std::string cacheKey =
            id + "@" + std::to_string(pointSize);
        auto it = fonts_.find(cacheKey);
        return it != fonts_.end() ? it->second : nullptr;
    }

    void AssetsManager::UnloadTexture(const std::string &id)
    {
        auto it = textures_.find(id);
        if (it == textures_.end())
            return;

        SDL_DestroyTexture(it->second);
        textures_.erase(it);
    }

    void AssetsManager::UnloadAllTextures()
    {
        for (auto &[id, tex] : textures_)
        {
            SDL_DestroyTexture(tex);
        }
        textures_.clear();
    }

} // namespace Axion
