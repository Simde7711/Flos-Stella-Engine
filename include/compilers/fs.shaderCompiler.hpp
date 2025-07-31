#pragma once

#include "fs.device.hpp"
#include "fs.shaderManager.hpp"
#include "compilers/fs.compilerBase.hpp"

// std
#include <filesystem>
#include <string>
#include <unordered_map>
#include <unordered_set>

// nlohmann
#include <nlohmann/json.hpp>

namespace fs
{
    class FsShaderCompiler: public FsCompilerBase
    {
        public:
            FsShaderCompiler(FsDevice *_device, std::string _sourcePath, std::string _destinationPath, nlohmann::json &_timeCache);
            ~FsShaderCompiler();

            void WatchForChanges(bool _startup = false) override;
            void Compile(const std::filesystem::path &_file) override;
        private:
            FsDevice *device;
            std::unordered_map<std::string, PipelineKey> shadersChanged;

            std::string HandleInclude(const std::filesystem::path &_file, std::unordered_set<std::filesystem::path> &_includedFiles, bool _isRoot = false);
            bool IsMainShader(const std::filesystem::path &_file);
    };
}
