#pragma once
#include <SDL.h>
#include <string>

namespace Axion
{

/**
 * @brief Interface for asset loading.
 * 
 * Provides access to asset loading functionality without
 * exposing the full AssetsManager implementation.
 */
class IAssetProvider
{
public:
    virtual ~IAssetProvider() = default;
    
    /** 
     * @brief Loads or retrieves a cached texture.
     * @param name Unique identifier for the texture
     * @param path File path relative to assets folder
     * @return SDL_Texture pointer (managed by AssetsManager)
     */
    virtual SDL_Texture* LoadTexture(const std::string& name, const std::string& path) = 0;
    
    /**
     * @brief Retrieves a previously loaded texture.
     * @param name Unique identifier for the texture
     * @return SDL_Texture pointer or nullptr if not found
     */
    virtual SDL_Texture* GetTexture(const std::string& name) const = 0;
};

} // namespace Axion
