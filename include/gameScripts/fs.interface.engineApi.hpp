#pragma once

#include "ECS/fs.types.hpp"

// std
#include <any>
#include <typeindex>

namespace fs 
{
    class FsIEngineAPI 
    {
        public:
            virtual ~FsIEngineAPI() = default;

            template<typename T>
            bool HasComponent(Entity _entity) const 
            {
                return HasComponentImpl(typeid(T), _entity);
            }

            template<typename T>
            T &GetComponent(Entity _entity) 
            {
                return std::any_cast<T>(GetComponentImpl(typeid(T), _entity));
            }

            template<typename T>
            void AddComponent(Entity entity, const T &&component) 
            {
                AddComponentImpl(typeid(T), entity, std::any(component));
            }

        protected:
            virtual bool HasComponentImpl(std::type_index _type, Entity _entity) const = 0;
            virtual std::any GetComponentImpl(std::type_index _type, Entity _entity) = 0;
            virtual void AddComponentImpl(std::type_index type, Entity entity, std::any component) = 0;
    };
}