#pragma once

#include <glm/glm.hpp>

struct Transform3D
{
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
    glm::vec3 speed = {0.0f, 0.0f, 0.0f};

    bool operator==(const Transform3D &other) const
    {
        return scale == other.scale &&
               position == other.position &&
               rotation == other.rotation &&
               speed == other.speed;
    }
};

struct Transform2D
{
    glm::vec2 scale = {1.0f, 1.0f};
    glm::vec2 position = {0.0f, 0.0f};
    float rotation = 0.0f;

    bool operator==(const Transform2D &other) const
    {
        return scale == other.scale &&
               position == other.position &&
               rotation == other.rotation;
    }
};

struct Transform3DSimple
{
    glm::ivec3 scale = {1, 1, 1};
    glm::ivec3 position = {0, 0, 0};
    glm::ivec3 rotation = {0, 0, 0};

    bool operator==(const Transform3DSimple &other) const
    {
        return scale == other.scale &&
               position == other.position &&
               rotation == other.rotation;
    }
};

struct Transform2DSimple
{
    glm::ivec2 position = {0, 0};
    glm::ivec2 scale = {1, 1};
    int rotation = 0;

    bool operator==(const Transform2DSimple &other) const
    {
        return scale == other.scale &&
               position == other.position &&
               rotation == other.rotation;
    }
};