#pragma once

#include "fs.device.hpp"
#include "fs.shaderManager.hpp"
#include "compilers/fs.compilerBase.hpp"

// std
#include <filesystem>
#include <string>
#include <vector>

namespace fs
{
    class FsShaderCompiler: public FsCompilerBase
    {
        public:
            FsShaderCompiler(FsDevice *_device, std::string _sourcePath, std::string _destinationPath);
            ~FsShaderCompiler();

            void WatchForChanges(bool _startup = false) override;
            void Compile(const std::filesystem::path &_file) override;
        private:
            FsDevice *device;

            std::vector<PipelineKey> shadersChanged;
    };
}
