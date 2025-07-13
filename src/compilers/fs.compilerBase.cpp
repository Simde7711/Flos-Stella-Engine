#include "compilers/fs.compilerBase.hpp"

// std
#include <string>

namespace fs 
{
    bool FsCompilerBase::CompareFileData(const std::filesystem::path &_file)
    {
        const auto name = _file.stem().string();
        const auto lastSrcWrite = std::filesystem::last_write_time(_file);

        auto it = filesWriteTime.find(name);
        if (it == filesWriteTime.end() || it->second != lastSrcWrite)
        {
            filesWriteTime[name] = lastSrcWrite;

            const std::string base = destinationPath + name;

            bool needsCompile = false;
            for (const std::string extension: extensionsOutput)
            {
                const std::filesystem::path searchedOutput = std::filesystem::path(base + extension);

                if (!std::filesystem::exists(searchedOutput) || std::filesystem::last_write_time(searchedOutput) < lastSrcWrite)
                {
                    needsCompile = true;
                    break;
                }
            }

            return needsCompile;
        }

        return false;
    }
}