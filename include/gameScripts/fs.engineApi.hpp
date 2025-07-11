#pragma once

#include "gameScripts/fs.interface.engineApi.hpp"
#include "ECS/fs.types.hpp"

// std
#include <any>
#include <typeindex>

namespace fs 
{
    class FsEngineAPI: public FsIEngineAPI
    {
        bool HasComponentImpl(std::type_index _type, Entity _entity) const override;

        std::any GetComponentImpl(std::type_index _type, Entity _entity) override;

        void AddComponentImpl(std::type_index type, Entity entity, std::any component) override;
    };
}