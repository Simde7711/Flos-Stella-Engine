#pragma once

#include "fs.device.hpp"
#include "compilers/fs.shaderCompiler.hpp"
#include "compilers/fs.dllCompiler.hpp"

// std
#include <memory>

// nlohmann
#include <nlohmann/json.hpp>

namespace fs 
{
    class FsCompilerManager
    {
        public:
            FsCompilerManager(FsDevice *_device, std::string _sourcePath, std::string _destinationShaderPath, std::string _destinationScriptPath);
            ~FsCompilerManager();
            
            void WatchForChanges();
        private:
            nlohmann::json timeCache;
            std::string timeCacheFilePath;

            std::unique_ptr<FsShaderCompiler> shaderCompiler;
            std::unique_ptr<FsDllCompiler> dllCompiler;

            void CreateTimeCache();
            void FlushFile();
    };
}