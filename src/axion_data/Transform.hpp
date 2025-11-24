#pragma once

#include "axion_math/Vector.hpp"

struct Transform3D
{
    Vector3 scale = {1.0f, 1.0f, 1.0f};
    Vector3 position = {0.0f, 0.0f, 0.0f};
    Vector3 rotation = {0.0f, 0.0f, 0.0f};
    Vector3 speed = {0.0f, 0.0f, 0.0f};

    bool operator==(const Transform3D &other) const
    {
        return scale.x == other.scale.x && scale.y == other.scale.y && scale.z == other.scale.z &&
               position.x == other.position.x && position.y == other.position.y && position.z == other.position.z &&
               rotation.x == other.rotation.x && rotation.y == other.rotation.y && rotation.z == other.rotation.z;
    }
};

struct Transform2D
{
    Vector2 scale = {1.0f, 1.0f};
    Vector2 position = {0.0f, 0.0f};
    float rotation = 0.0f;

    bool operator==(const Transform2D &other) const
    {
        return scale.x == other.scale.x && scale.y == other.scale.y &&
               position.x == other.position.x && position.y == other.position.y &&
               rotation == other.rotation;
    }
};

struct Transform3DSimple
{
    Vector3Simple scale = {1, 1, 1};
    Vector3Simple position = {0, 0, 0};
    Vector3Simple rotation = {0, 0, 0};

    bool operator==(const Transform3DSimple &other) const
    {
        return scale.x == other.scale.x && scale.y == other.scale.y && scale.z == other.scale.z &&
               position.x == other.position.x && position.y == other.position.y && position.z == other.position.z &&
               rotation.x == other.rotation.x && rotation.y == other.rotation.y && rotation.z == other.rotation.z;
    }
};

struct Transform2DSimple
{
    Vector2Simple scale = {1, 1};
    Vector2Simple position = {0, 0};
    int rotation = 0;

    bool operator==(const Transform2DSimple &other) const
    {
        return scale.x == other.scale.x && scale.y == other.scale.y &&
               position.x == other.position.x && position.y == other.position.y &&
               rotation == other.rotation;
    }
};