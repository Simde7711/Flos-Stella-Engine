#pragma once

#include "ECS/fs.types.hpp"

// std
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

namespace fs 
{
    struct ScriptBase
    {
        virtual ~ScriptBase() = default;

        Entity self;

        virtual void Init() = 0;
        virtual void Update() = 0;
    };

    struct ScriptInstance 
    {
        void* dllHandle = nullptr;                       
        std::string dllPath;                             
    };


    struct ScriptComponent 
    {
        std::unordered_map<std::type_index, std::unique_ptr<ScriptBase>> scripts;

        template<typename T>
        bool HasScript() const 
        {
            return scripts.find(typeid(T)) != scripts.end();
        }

        template<typename T>
        T* GetScript() 
        {
            for (auto& [name, ptr] : scripts) 
            {
                if (auto casted = dynamic_cast<T*>(ptr.get()))
                    return casted;
            }
            return nullptr;
        }

        template<typename T>
        void AddScript(std::unique_ptr<T> script)
        {
            scripts[std::type_index(typeid(T))] = std::move(script);
        }
    };
}