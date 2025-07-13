#pragma once

#include "gameScripts/fs.engineApi.hpp"

// std
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace fs 
{
    struct Script 
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