#pragma once

#include "compilers/fs.compilerBase.hpp"

// std
#include <filesystem>
#include <string>
#include <vector>

namespace fs 
{
    class FsDllCompiler: public FsCompilerBase
    {
        public:
            FsDllCompiler(std::string _sourcePath, std::string _destinationPath);
            ~FsDllCompiler();

            void WatchForChanges(bool _startup = false) override;
            void Compile(const std::filesystem::path &_file) override;
        private:

            std::vector<std::string> scriptsChanged;
    };
}