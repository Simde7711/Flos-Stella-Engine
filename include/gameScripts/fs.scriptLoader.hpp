#pragma once

#include "ECS/components/fs.component.script.hpp"

// std
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>


#ifdef _WIN32
    #include <Windows.h>
#else
    #include <dlfcn.h>
#endif

namespace fs 
{
    class ScriptLoader 
    {
        public:
            using ScriptFactory = std::function<std::unique_ptr<ScriptBase>()>;

            // Load a DLL and register its scripts
            bool LoadLibrary(const std::string& dllPath);

            // Create a script instance by class name
            std::unique_ptr<fs::ScriptBase> CreateScript(const std::string& className);

            // Unload all DLLs (optional, to be called on shutdown)
            void UnloadAll();

        private:
            struct ScriptLibrary {
        #ifdef _WIN32
                HMODULE dllHandle = nullptr;
        #else
                void* dllHandle = nullptr;
        #endif
                std::string dllPath;

                // className -> factory function
                std::unordered_map<std::string, ScriptFactory> factories;
            };

            std::unordered_map<std::string, ScriptLibrary> libraries; // keyed by dllPath
    };
}