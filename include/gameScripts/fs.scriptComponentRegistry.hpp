#pragma once

#include "ECS/fs.types.hpp"
#include "ECS/fs.coordinator.hpp"

// std
#include <any>
#include <functional>
#include <stdexcept>
#include <unordered_map>
#include <typeindex>

namespace fs 
{

    class ScriptComponentRegistry {
    public:
        using HasFunc = std::function<bool(Entity)>;
        using GetFunc = std::function<std::any(Entity)>;
        using AddFunc = std::function<void(Entity, std::any)>;

        static ScriptComponentRegistry& Get() {
            static ScriptComponentRegistry instance;
            return instance;
        }

        template<typename T>
        void Register() {
            hasMap[typeid(T)] = [](Entity e) {
                return FsCoordinator::GetInstance().HasComponent<T>(e);
            };

            getMap[typeid(T)] = [](Entity e) -> std::any {
                return FsCoordinator::GetInstance().GetComponent<T>(e);
            };

            addMap[typeid(T)] = [](Entity e, std::any val) {
                FsCoordinator::GetInstance().AddComponent<T>(e, std::any_cast<T>(val));
            };
        }

        bool Has(std::type_index type, Entity e) const {
            auto it = hasMap.find(type);
            if (it != hasMap.end()) return it->second(e);
            return false;
        }

        std::any Get(std::type_index type, Entity e) const {
            auto it = getMap.find(type);
            if (it != getMap.end()) return it->second(e);
            throw std::runtime_error("Component not registered for scripting");
        }

        void Add(std::type_index type, Entity e, std::any value) const {
            auto it = addMap.find(type);
            if (it != addMap.end()) {
                it->second(e, std::move(value));
                return;
            }
            throw std::runtime_error("Component add function not registered for scripting");
        }

    private:
        std::unordered_map<std::type_index, HasFunc> hasMap;
        std::unordered_map<std::type_index, GetFunc> getMap;
        std::unordered_map<std::type_index, AddFunc> addMap;
    };
}