#include "compilers/fs.compilerBase.hpp"

// std
#include <string>
#include <algorithm>
#include <fstream>

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

    std::vector<std::string> FsCompilerBase::ParseIncludes(const std::filesystem::path &_file) 
    {
        std::ifstream file(_file);
        std::vector<std::string> includes;

        std::string line;
        while (std::getline(file, line)) 
        {
            if (line.find("#include") != std::string::npos) 
            {
                int first = line.find_first_of("\"");
                int last = line.find_last_of("\"");

                if (first == -1 || last == -1)
                {  
                    continue;
                }

                std::string includeContent = line.substr(first + 1, last - first - 1);

                std::filesystem::path includePath(includeContent);
                std::string includeFilename = includePath.filename().string();

                includes.push_back(includeFilename);
            }
        }

        return includes;
    }

    void FsCompilerBase::GetFilesMap()
    {
        for (const auto &file : std::filesystem::recursive_directory_iterator(sourcePath)) 
        {
            if (!file.is_regular_file()) continue;
            if (filesMap.find(file.path().filename().string()) != filesMap.end()) continue;

            auto ext = file.path().extension();
            if (std::find(extensionsInput.begin(), extensionsInput.end(), ext) != extensionsInput.end())  
            {
                filesMap[file.path().filename().string()] = (std::filesystem::absolute(file.path()));
            }
        }
    }
}