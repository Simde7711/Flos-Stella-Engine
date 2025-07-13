#include "compilers/fs.shaderCompiler.hpp"

// std 
#include <iostream>
#include <cstdlib>

namespace fs
{
    FsShaderCompiler::FsShaderCompiler(FsDevice *_device, std::string _sourcePath, std::string _destinationPath)
    {
        device = _device;

        sourcePath = _sourcePath;
        std::cout << "Le sourcePath est: " << sourcePath << '\n';

        destinationPath = _destinationPath;
        std::cout << "Le destinationPath est: " << destinationPath << '\n';

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
                // faire en sorte de cheque les autres types de shader file
                if (file.path().filename().extension() != ".glsl") continue;

                if (std::filesystem::is_regular_file(file)) 
                {
                    if (CompareFileData(file.path()))
                    {
                        std::cout << "[FsShaderCompiler] Compilation du shader: " << file.path() << '\n';

                        Compile(file.path());
                    }
                }
            }
            
            if (!shadersChanged.empty() && !_startup)
            {
                std::cout << "[FsShaderCompiler] Debut de la recreation des pipelines." << '\n';

                vkDeviceWaitIdle(device->device());
                for (const auto &key : shadersChanged)
                {
                    std::cout << "[FsShaderCompiler] Recreation des pipelines avec les shaders path: " 
                        << key.vertShaderPath 
                        << " et " 
                        << key.fragShaderPath
                        << '\n';

                    FsShaderManager::GetInstance().RecreatePipelinesFromShaderPath(key);
                }
            }
            
            shadersChanged.clear();
        } 
        catch (const std::filesystem::filesystem_error &e) 
        {
            std::cerr << "[FsShaderCompiler] Error: " << e.what() << std::endl;
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

            if (std::system(command.c_str()))
            {
                std::cerr << "[FsShaderCompiler] Shader compilation failed (" << stage.stage << "): " << inputFile << "\n";
                return;
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
