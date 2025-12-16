#pragma once

class GameObject;

class ICollisionListener
{
public:
    ~ICollisionListener() = default;

    virtual void OnCollisionEnter(GameObject &other) {};
    virtual void OnCollisionExit(GameObject &other) {};

    virtual void OnTriggerEnter(GameObject &other) {};
    virtual void OnTriggerExit(GameObject &other) {};
};