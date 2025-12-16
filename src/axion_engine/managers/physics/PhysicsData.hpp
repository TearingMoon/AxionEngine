#pragma once
#include <unordered_set>

class GameObject;
class ColliderComponent;

struct PairHash
{
    std::size_t operator()(const std::pair<ColliderComponent *, ColliderComponent *> &p) const
    {
        auto h1 = std::hash<ColliderComponent *>{}(p.first);
        auto h2 = std::hash<ColliderComponent *>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};