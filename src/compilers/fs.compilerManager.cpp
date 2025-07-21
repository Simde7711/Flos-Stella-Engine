#include "compilers/fs.compilerManager.hpp"
#include "fs.device.hpp"

// std
#include <memory>

namespace fs 
{
    FsCompilerManager::FsCompilerManager(FsDevice *_device ,std::string _sourcePath, std::string _destinationShaderPath, std::string _destinationScriptPath) 
    {
        // shader et dll compiler
        shaderCompiler = std::make_unique<FsShaderCompiler>(_device, _sourcePath, _destinationShaderPath);
        dllCompiler = std::make_unique<FsDllCompiler>(_sourcePath, _destinationShaderPath);
    }

    FsCompilerManager::~FsCompilerManager()
    {

    }

    void FsCompilerManager::WatchForChanges()
    {
        shaderCompiler->WatchForChanges();
        dllCompiler->WatchForChanges();
    }
}