#pragma once

class ICollisionListener
{
public:
    ~ICollisionListener() = default;

    virtual void OnCollisionEnter(ICollisionListener &other) {};
    virtual void OnCollisionExit(ICollisionListener &other) {};

    virtual void OnTriggerEnter(ICollisionListener &other) {};
    virtual void OnTriggerExit(ICollisionListener &other) {};
};