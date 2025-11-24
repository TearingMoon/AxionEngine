#pragma once
#include <vector>
#include <memory>

#include "axion_engine/structure/ContextAware.hpp"
#include "axion_engine/core/EngineContext.hpp"
#include "axion_engine/runtime/components/Component.hpp"
#include "axion_engine/runtime/components/transform/TransformComponent.hpp"
#include "axion_engine/runtime/components/collider/BaseColliderComponent.hpp"


class Scene;

class GameObject
{
public:
    GameObject(Scene &parentScene);
    ~GameObject() = default;

    void Tick();
    void FixedUpdate();

    void Mounted();
    void Destroy();

    void Enable();
    void Disable();

    template <typename T, typename... Args>
    T *AddComponent(Args &&...args);

    template <typename T>
    T *GetComponent() const;

    void MarkAsDestroyed() { isDestroyed_ = true; }

    bool IsEnabled() { return isEnabled_; }
    bool IsDestroyed() { return isDestroyed_; }

    bool HasCollider() const;

private:
    Scene &parentScene_;
    TransformComponent *transform_;

    EngineContext &ctx_();

    bool isEnabled_ = false;
    bool isDestroyed_ = false;
    bool isFirsUpdate_ = true;

    std::vector<std::unique_ptr<Component>> components_;
};

template <typename T, typename... Args>
inline T *GameObject::AddComponent(Args &&...args)
{
    static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

    auto comp = std::make_unique<T>(std::forward<Args>(args)...);
    comp->Bind(this);

    T *rawPtr = comp.get();
    components_.emplace_back(std::move(comp));
    return rawPtr;
}

template <typename T>
inline T *GameObject::GetComponent() const
{
    for (const auto &comp : components_)
    {
        if (auto casted = dynamic_cast<T *>(comp.get()))
        {
            return casted;
        }
    }
    return nullptr;
}
