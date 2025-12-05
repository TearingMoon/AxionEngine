#pragma once

#include <glm/glm.hpp>


#include "axion_engine/runtime/components/Component.hpp"

class RigidBody : public Component
{
    public:

    float mass = 1.0f;

    void AddForce(float x, float y, float z)
    {
        velocity += glm::vec3(x, y, z) / mass;
    }

    private:
    glm::vec3 velocity;
};