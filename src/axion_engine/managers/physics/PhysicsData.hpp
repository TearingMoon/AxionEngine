#pragma once
#include <unordered_set>

namespace Axion
{

class GameObject;
class ColliderComponent;

/**
 * @brief Hash function for collider pointer pairs.
 */
struct PairHash
{
    std::size_t operator()(const std::pair<ColliderComponent*, ColliderComponent*>& p) const
    {
        auto h1 = std::hash<ColliderComponent*>{}(p.first);
        auto h2 = std::hash<ColliderComponent*>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

/**
 * @brief Hash function for size_t pairs (GameObject IDs).
 */
struct PairHashSize
{
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const
    {
        return std::hash<T1>()(pair.first) ^ (std::hash<T2>()(pair.second) << 1);
    }
};

/**
 * @brief Stores collider pair with associated GameObject ID for validation.
 */
struct ColliderPairWithId
{
    size_t objId;
    GameObject* obj;
    ColliderComponent* collider;
};

} // namespace Axion