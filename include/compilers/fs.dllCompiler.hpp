#pragma once

#include "compilers/fs.compilerBase.hpp"

// std
#include <filesystem>
#include <string>
#include <vector>

// nlohmann
#include <nlohmann/json.hpp>

namespace fs 
{
    class FsDllCompiler: public FsCompilerBase
    {
        public:
            FsDllCompiler(std::string _sourcePath, std::string _destinationPath, nlohmann::json &_timeCache);
            ~FsDllCompiler();

            void WatchForChanges(bool _startup = false) override;
            void Compile(const std::filesystem::path &_file) override;
        private:
            std::vector<std::string> engineIncludes;
            std::vector<std::string> scriptsChanged;
    };
}