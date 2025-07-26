#include "compilers/fs.dllCompiler.hpp"
#include "fs.logger.hpp"

// std
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace fs 
{
    FsDllCompiler::FsDllCompiler(std::string _sourcePath, std::string _destinationPath) 
    {
        sourcePath = _sourcePath;
        FsLogger::GetInstance().Log(LogType::System, "[FsDllCompiler] Le sourcePath est: " + sourcePath);

        destinationPath = _destinationPath;
        FsLogger::GetInstance().Log(LogType::System, "[FsDllCompiler] Le destinationPath est: " + destinationPath);

        engineIncludes = { "fs.engineApi.hpp" };
        extensionsInput = { ".cpp", ".hpp" }; 
        extensionsOutput = { ".dll", };

        WatchForChanges(true);
    }

    FsDllCompiler::~FsDllCompiler() 
    {

    }

    void FsDllCompiler::WatchForChanges(bool _startup)
    {
        try 
        {
            GetFilesMap();
            GetReverseDependencyGraph(engineIncludes);

            std::vector<std::filesystem::path> dirtyFiles;

            for (const auto &it : filesMap) 
            {
                if (CompareFileData(it.second)) 
                {
                    dirtyFiles.push_back(it.second);
                }
            }
            
            for (size_t i = 0; i < dirtyFiles.size(); ++i) 
            {
                const auto &file = dirtyFiles[i];
                
                auto dependencies = reverseDependencyGraph.find(file);

                if (dependencies != reverseDependencyGraph.end()) 
                {
                    for (const auto &dependency : dependencies->second) 
                    {
                        if (std::find(dirtyFiles.begin(), dirtyFiles.end(), dependency) == dirtyFiles.end()) 
                        {
                            dirtyFiles.push_back(dependency);
                        }
                    }
                }
            }

            for (const auto &file : dirtyFiles) 
            {
                if (file.extension() == ".hpp") continue;

                FsLogger::GetInstance().Log(LogType::System, "[FsDllCompiler] Compilation du script: " + file.string());
                Compile(file);
            }
            

            // manque la partie du script manager
            scriptsChanged.clear();
        } 
        catch (const std::filesystem::filesystem_error &e) 
        {
            FsLogger::GetInstance().Log(LogType::Error, std::string("[FsDllCompiler] Error: ") + e.what());
        }
    }

    void FsDllCompiler::Compile(const std::filesystem::path &_file)
    {
        std::filesystem::path outputDll = _file.stem().string() + ".dll";
        const std::string outputBase = destinationPath + outputDll.string();

        std::filesystem::path projectRoot = sourcePath + "/..";
        std::filesystem::path includePath = projectRoot / "include";

        std::string command =
            "g++ -shared -fPIC \"" 
            + _file.string() 
            + "\" " 
            + "-I\"" 
            + includePath.string() 
            + "\"" 
            + " -o \""
            + outputBase 
            + "\"";

        int result = std::system(command.c_str());

        if (result != 0)
        {
            FsLogger::GetInstance().Log(LogType::Error, "[FsDllCompiler] Script compilation failed: " + _file.filename().string() + " with code: " + std::to_string(result));
        }
        else
        {
            FsLogger::GetInstance().Log(LogType::System, "[FsDllCompiler] Script compilation successful: " + _file.filename().string());
        }

        scriptsChanged.push_back(outputDll.string());
    }
}