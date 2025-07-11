#include "gameScripts/fs.engineApi.hpp"
#include "gameScripts/fs.scriptComponentRegistry.hpp"

// std
#include <any>
#include <typeindex>

namespace fs 
{
    bool FsEngineAPI::HasComponentImpl(std::type_index _type, Entity _entity) const
    {
        return ScriptComponentRegistry::Get().Has(_type, _entity);
    }

    std::any FsEngineAPI::GetComponentImpl(std::type_index _type, Entity _entity) 
    {
        return ScriptComponentRegistry::Get().Get(_type, _entity);
    }

    void FsEngineAPI::AddComponentImpl(std::type_index type, Entity entity, std::any component) 
    {
        ScriptComponentRegistry::Get().Add(type, entity, std::move(component));
    }
}