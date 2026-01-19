#pragma once

#include "axion_engine/Axion.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <cmath>

namespace Axion
{

class InfiniteBackgroundScript : public ScriptableComponent
{
public:
    float tileSize = 1024.0f;
    std::vector<GameObject*> tiles;

    void Update(EngineContext &context) override
    {
        auto *owner = GetOwner();
        if (!owner || owner->IsDestroyed()) return;
        
        auto *scene = context.scene->GetCurrentScene();
        if (!scene) return;
        
        auto *camera = scene->GetCurrentCamera();
        if (!camera) return;
        
        auto *camOwner = camera->GetOwner();
        if (!camOwner || camOwner->IsDestroyed()) return;
        
        auto *camTr = camOwner->GetTransform();
        if (!camTr) return;
        
        glm::vec3 camPos = camTr->GetPosition();
        
        // Calculate center chunk - add half tile to center the grid around camera
        float halfTile = tileSize * 0.5f;
        float chunkX = std::floor((camPos.x + halfTile) / tileSize) * tileSize;
        float chunkY = std::floor((camPos.y + halfTile) / tileSize) * tileSize;
        
        int index = 0;
        for (int y = -1; y <= 1; y++)
        {
            for (int x = -1; x <= 1; x++)
            {
                if (index < tiles.size() && tiles[index] && !tiles[index]->IsDestroyed())
                {
                    auto *tileTr = tiles[index]->GetTransform();
                    if (tileTr)
                    {
                        float tileX = chunkX + x * tileSize;
                        float tileY = chunkY + y * tileSize;
                        glm::vec3 currentPos = tileTr->GetPosition();
                        tileTr->SetPosition({tileX, tileY, currentPos.z});
                    }
                }
                index++;
            }
        }
    }
};

} // namespace Axion
