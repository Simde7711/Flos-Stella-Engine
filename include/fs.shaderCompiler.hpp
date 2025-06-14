#pragma once

#include "fs.device.hpp"
#include "fs.shaderManager.hpp"

// std
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace fs
{
    class FsShaderCompiler
    {
        public:
            void Init(VkDevice _device, std::string _sourcePath, std::string _destinationPath);
            void WatchForChanges(bool _startup = false);
            bool CompareFileData(const std::filesystem::__cxx11::path &_file);
            void CompileShader(const std::filesystem::__cxx11::path &_file);

        private:
            VkDevice device;

            std::string sourcePath;
            std::string destinationPath;

            std::unordered_map<std::string, std::filesystem::file_time_type> filesWriteTime;
            std::vector<PipelineKey> shadersChanged;
    };

    // pour faire une instance unique dans le cpp
    extern FsShaderCompiler shaderCompiler;
}
