#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <SDL.h>
#include <SDL_image.h>

#include "axion_engine/structure/ContextAware.hpp"
#include "axion_engine/platform/window/Window.hpp"

class AssetsManager : ContextAware
{
public:
    AssetsManager(EngineContext &ctx);
    ~AssetsManager();

    SDL_Texture *LoadTexture(const std::string &id, const std::string &relativePath);

    SDL_Texture *GetTexture(const std::string &id) const;

    void UnloadTexture(const std::string &id);

    void UnloadAllTextures();

private:
    std::string assetsRoot_; // TODO: Initialize this with the correct assets root path
    std::unordered_map<std::string, SDL_Texture *> textures_;

    inline std::string GetAssetsRoot();
};