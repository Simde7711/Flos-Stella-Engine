#include "compilers/fs.compilerManager.hpp"
#include "fs.device.hpp"
#include "fs.logger.hpp"

// std
#include <memory>
#include <fstream>

// nlohmann
#include <nlohmann/json.hpp>

namespace fs 
{
    FsCompilerManager::FsCompilerManager(FsDevice *_device ,std::string _sourcePath, std::string _destinationShaderPath, std::string _destinationScriptPath) 
    {
        timeCacheFilePath = "fileWriteTimeCache.json";
        CreateTimeCache();

        // shader et dll compiler
        shaderCompiler = std::make_unique<FsShaderCompiler>(_device, _sourcePath, _destinationShaderPath, timeCache["shaders"]);
        dllCompiler = std::make_unique<FsDllCompiler>(_sourcePath, _destinationScriptPath, timeCache["scripts"]);

        FlushFile();
    }

    FsCompilerManager::~FsCompilerManager()
    {

    }

    void FsCompilerManager::WatchForChanges()
    {
        shaderCompiler->WatchForChanges();
        dllCompiler->WatchForChanges();

        FlushFile();
    }

    void FsCompilerManager::CreateTimeCache()
    {
        if (std::filesystem::exists(timeCacheFilePath)) 
        {
            std::ifstream file(timeCacheFilePath);
            try
            {
                timeCache = nlohmann::json::parse(file, nullptr, true);
            }
            catch (const nlohmann::json::parse_error& e) 
            {
                FsLogger::GetInstance().Log(LogType::Warning, "[FsCompilerManager] Impossible de parse le contenu du fichier: " + timeCacheFilePath);

                timeCache = nlohmann::json::object();
            }
        } 
        else 
        {
            timeCache = nlohmann::json::object();

            timeCache = {
                {"shaders", nlohmann::json::object()},
                {"scripts", nlohmann::json::object()}
            };
        }
    }

    void FsCompilerManager::FlushFile() 
    {
        std::ofstream file(timeCacheFilePath);
        if (file.is_open()) 
        {
            file << std::setw(4) << timeCache << std::endl;
        } 
        else 
        {
            FsLogger::GetInstance().Log(LogType::Warning, "[FsCompilerManager] Impossible d'ouvrir le fichier: " + timeCacheFilePath);
        }
    }
}