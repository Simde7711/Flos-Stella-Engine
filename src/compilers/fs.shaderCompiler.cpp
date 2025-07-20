#include "compilers/fs.shaderCompiler.hpp"
#include "fs.logger.hpp"

// std 
#include <cstdlib>
#include <filesystem>

namespace fs
{
    FsShaderCompiler::FsShaderCompiler(FsDevice *_device, std::string _sourcePath, std::string _destinationPath)
    {
        device = _device;

        sourcePath = _sourcePath;
        FsLogger::GetInstance().Log(LogType::System, "[FsShaderCompiler] Le sourcePath est: " + sourcePath);

        destinationPath = _destinationPath;
        FsLogger::GetInstance().Log(LogType::System, "[FsShaderCompiler] Le destinationPath est: " + destinationPath);

        // manque. vert et .frag
        extensionsInput = { ".glsl" }; 
        extensionsOutput = { ".vert.spv", ".frag.spv" };

        WatchForChanges(true);
    }

    FsShaderCompiler::~FsShaderCompiler()
    {
        device = nullptr;
    }

    void FsShaderCompiler::WatchForChanges(bool _startup)
    {
        try 
        {
            for (const auto &file : std::filesystem::recursive_directory_iterator(sourcePath)) 
            {
                if (std::find(extensionsInput.begin(), extensionsInput.end(), file.path().filename().extension()) == extensionsInput.end()) 
                {
                    continue;
                }

                if (std::filesystem::is_regular_file(file)) 
                {
                    if (CompareFileData(file.path()))
                    {
                        FsLogger::GetInstance().Log(LogType::System, "[FsShaderCompiler] Compilation du shader: " + file.path().string());

                        Compile(file.path());
                    }
                }
            }
            
            if (!shadersChanged.empty() && !_startup)
            {
                FsLogger::GetInstance().Log(LogType::System, "[FsShaderCompiler] Debut de la recreation des pipelines.");

                vkDeviceWaitIdle(device->device());
                for (const auto &key : shadersChanged)
                {
                    FsLogger::GetInstance().Log(LogType::System, "[FsShaderCompiler] Recreation des pipelines avec les shaders path: " 
                        + key.vertShaderPath 
                        + " et " 
                        + key.fragShaderPath
                    );

                    FsShaderManager::GetInstance().RecreatePipelinesFromShaderPath(key);
                }
            }
            
            shadersChanged.clear();
        } 
        catch (const std::filesystem::filesystem_error &e) 
        {
            FsLogger::GetInstance().Log(LogType::Error, std::string("[FsShaderCompiler] Error: ") + e.what());
        }
    }

    // TODO: Faire en sorte d'être capable de compiler dans .glsl, .vert, .frag sans causer de double recréation de pipelines.
    void FsShaderCompiler::Compile(const std::filesystem::path &_file)
    {
        const std::string glslcPath = "C:\\VulkanSDK\\1.4.309.0\\Bin\\glslc.exe";
        const std::string inputFile = _file.string();
        const std::string outputBase = destinationPath + _file.stem().string();

        struct ShaderStage
        {
            std::string define;
            std::string stage;
            std::string extension;
        };

        PipelineKey pipelineKey{};
        std::vector<ShaderStage> stages;

        if (_file.extension() == ".glsl") 
        {
            stages = 
            {
                {"VERTEX_SHADER", "vert", ".vert.spv"},
                {"FRAGMENT_SHADER", "frag", ".frag.spv"}
            };
        }

        for (const auto &stage : stages)
        {
            std::string command = glslcPath +
                " -D" + stage.define +
                " -fshader-stage=" + stage.stage +
                " " + inputFile +
                " -o " + outputBase + stage.extension;

            
            int result = std::system(command.c_str());

            if (result != 0)
            {
                FsLogger::GetInstance().Log(LogType::Error, "[FsDllCompiler] Shader compilation failed (" + stage.stage + "): " + inputFile + " with code: " + std::to_string(result));
                return;
            }
            else
            {
                FsLogger::GetInstance().Log(LogType::System, "[FsDllCompiler] Shader compilation successful (" + stage.stage + "): " + inputFile);
            }

            if (stage.extension == ".vert.spv")
            {
                pipelineKey.vertShaderPath = outputBase + stage.extension;
            }
            else if (stage.extension == ".frag.spv")
            {
                pipelineKey.fragShaderPath = outputBase + stage.extension;
            }

        }

        shadersChanged.push_back(pipelineKey);
    }
}
