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

// Hash function for pairs of size_t (GameObject IDs)
struct PairHashSize
{
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &pair) const
    {
        return std::hash<T1>()(pair.first) ^ (std::hash<T2>()(pair.second) << 1);
    }
};

// Store ID with pointers to verify validity during collision detection
struct ColliderPairWithId {
    size_t objId;
    GameObject *obj;
    ColliderComponent *collider;
};