#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <filesystem>

#include "axion_engine/structure/ContextAware.hpp"
#include "axion_engine/platform/window/Window.hpp"
#include "axion_engine/core/interfaces/IAssetProvider.hpp"

#include "axion_utilities/path/PathFinder.hpp"

namespace Axion
{

/**
 * @brief Manages loading and caching of game assets.
 * 
 * AssetsManager provides texture loading from files and maintains a cache
 * of loaded textures to avoid redundant disk access. Textures are identified
 * by user-defined string IDs.
 * 
 * Implements IAssetProvider for decoupled asset loading.
 */
class AssetsManager : public ContextAware, public IAssetProvider
{
public:
    AssetsManager(EngineContext& ctx);
    ~AssetsManager();

    /**
     * @brief Loads a texture from file and caches it.
     * @param id Unique identifier for the texture
     * @param relativePath Path relative to the assets folder
     * @return Pointer to the loaded texture, or nullptr on failure
     */
    SDL_Texture* LoadTexture(const std::string& id, const std::string& relativePath) override;

    /**
     * @brief Retrieves a previously loaded texture.
     * @param id Texture identifier
     * @return Pointer to the texture, or nullptr if not found
     */
    SDL_Texture* GetTexture(const std::string& id) const override;

    TTF_Font* LoadFont(const std::string& id,
                       const std::string& relativePath,
                       int pointSize);
    TTF_Font* GetFont(const std::string& id, int pointSize) const;

    /**
     * @brief Unloads a specific texture and frees its memory.
     * @param id Texture identifier to unload
     */
    void UnloadTexture(const std::string& id);

    /** @brief Unloads all cached textures. */
    void UnloadAllTextures();

private:
    std::string assetsRoot_;
    std::unordered_map<std::string, SDL_Texture*> textures_;
    std::unordered_map<std::string, TTF_Font*> fonts_;
    std::unordered_set<std::string> failedFonts_;
};

} // namespace Axion