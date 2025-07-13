#pragma once

#include "gameScripts/fs.interface.engineApi.hpp"
#include "ECS/fs.types.hpp"

// std
#include <any>
#include <typeindex>

// macro
#define FS_REGISTER_SCRIPT(CLASS_NAME)                          \
extern "C" __declspec(dllexport) fs::ScriptBase* CreateScript() \
{ return new CLASS_NAME(); }                                    \
                                                                \
extern "C" __declspec(dllexport) void DestroyScript(fs::ScriptBase* ptr) \
{ delete ptr; }

namespace fs 
{
    struct ScriptBase
    {
        virtual ~ScriptBase() = default;

        Entity self;

        virtual void Init() = 0;
        virtual void Update() = 0;
    };

    class FsEngineAPI: public FsIEngineAPI
    {   
        protected:
            bool HasComponentImpl(std::type_index _type, Entity _entity) const override;
            std::any GetComponentImpl(std::type_index _type, Entity _entity) override;
            void AddComponentImpl(std::type_index type, Entity entity, std::any component) override;
    };
}
