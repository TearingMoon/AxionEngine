#pragma once
#include <unordered_set>

class GameObject;
class ColliderComponent;

struct ColliderEntry
{
    ColliderComponent *collider;
    GameObject *owner;
    bool isTrigger;
};

struct GO_Pair
{
    GameObject *a;
    GameObject *b;

    bool operator==(const GO_Pair &other) const noexcept
    {
        return a == other.a && b == other.b;
    }
};

struct GO_Pair_Hash
{
    std::size_t operator()(const GO_Pair &p) const noexcept
    {
        auto ha = std::hash<GameObject *>{}(p.a);
        auto hb = std::hash<GameObject *>{}(p.b);
        return ha ^ (hb + 0x9e3779b9 + (ha << 6) + (ha >> 2));
    }
};

GO_Pair static MakeOrderedPair(GameObject *a, GameObject *b)
{
    if (a < b)
        return {a, b};
    return {b, a};
}