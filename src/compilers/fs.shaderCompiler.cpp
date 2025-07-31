#include "compilers/fs.shaderCompiler.hpp"
#include "fs.logger.hpp"
#include "fs.shaderManager.hpp"

// std 
#include <cstdlib>
#include <unordered_set>
#include <filesystem>
#include <sstream>

namespace fs
{
    FsShaderCompiler::FsShaderCompiler(FsDevice *_device, std::string _sourcePath, std::string _destinationPath, nlohmann::json &_timeCache): FsCompilerBase(_timeCache)
    {
        device = _device;

        sourcePath = _sourcePath;
        FsLogger::GetInstance().Log(LogType::System, "[FsShaderCompiler] Le sourcePath est: " + sourcePath);

        destinationPath = _destinationPath;
        FsLogger::GetInstance().Log(LogType::System, "[FsShaderCompiler] Le destinationPath est: " + destinationPath);

        extensionsInput = { ".glsl", ".vert", ".frag" }; 
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
            GetFilesMap();
            GetReverseDependencyGraph();

            std::vector<std::filesystem::path> dirtyFiles;

            for (const auto &it : filesMap) 
            {
                if (CompareFileData(it.second, IsMainShader(it.second))) 
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
                if (IsMainShader(file))
                {
                    FsLogger::GetInstance().Log(LogType::System, "[FsShaderCompiler] Compilation du shader: " + file.string());
                    Compile(file);
                }
            }
            
            if (!shadersChanged.empty() && !_startup)
            {
                FsLogger::GetInstance().Log(LogType::System, "[FsShaderCompiler] Debut de la recreation des pipelines.");

                vkDeviceWaitIdle(device->device());
                for (const auto &shader : shadersChanged)
                {
                    PipelineKey key = shader.second;

                    std::ostringstream msg;
                    msg << "[FsShaderCompiler] Recreation des pipelines avec le shader path: ";

                    bool wroteAny = false;

                    if (!key.vertShaderPath.empty()) 
                    {
                        msg << key.vertShaderPath;
                        wroteAny = true;
                    }

                    if (!key.fragShaderPath.empty()) 
                    {
                        if (wroteAny) msg << " et ";
                        msg << key.fragShaderPath;
                    }

                    FsLogger::GetInstance().Log(LogType::System, msg.str());

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

    void FsShaderCompiler::Compile(const std::filesystem::path &_file)
    {
        const std::string glslcPath = "C:\\VulkanSDK\\1.4.309.0\\Bin\\glslc.exe";
        const std::string outputBase = destinationPath + _file.stem().string();

        PipelineKey pipelineKey;

        auto it = shadersChanged.find(outputBase);
        if (it != shadersChanged.end()) 
        {
            pipelineKey = it->second;
        }    
        
        struct ShaderStage
        {
            std::string define;
            std::string stage;
            std::string extension;
        };

        std::vector<ShaderStage> stages;

        if (_file.extension() == ".glsl") 
        {
            stages = 
            {
                {"VERTEX_SHADER", "vert", ".vert.spv"},
                {"FRAGMENT_SHADER", "frag", ".frag.spv"}
            };
        }

        if (_file.extension() == ".vert") 
        {
            stages = 
            {
                {"VERTEX_SHADER", "vert", ".vert.spv"}
            };
        }

        if (_file.extension() == ".frag") 
        {
            stages = 
            {
                {"FRAGMENT_SHADER", "frag", ".frag.spv"}
            };
        }

        for (const auto &stage : stages)
        {
            std::string tempFilePath = outputBase + "." + stage.stage + ".glsl";

            std::unordered_set<std::filesystem::path> includedFiles;
            includedFiles.insert(_file);
            std::string processedSource = HandleInclude(_file, includedFiles, true);
            std::ofstream tempFile(tempFilePath);
            tempFile << processedSource;
            tempFile.close();

            std::string command = glslcPath +
                " -D" + stage.define +
                " -fshader-stage=" + stage.stage +
                " " + tempFilePath +
                " -o " + outputBase + stage.extension;

            
            int result = std::system(command.c_str());

            if (result != 0)
            {
                FsLogger::GetInstance().Log(LogType::Error, "[FsShaderCompiler] Shader compilation failed (" + stage.stage + "): " + _file.filename().string() + " with code: " + std::to_string(result));
                return;
            }
            else
            {
                FsLogger::GetInstance().Log(LogType::System, "[FsShaderCompiler] Shader compilation successful (" + stage.stage + "): " + _file.filename().string());
            }

            if (stage.extension == ".vert.spv")
            {
                pipelineKey.vertShaderPath = outputBase + stage.extension;
            }
            else if (stage.extension == ".frag.spv")
            {
                pipelineKey.fragShaderPath = outputBase + stage.extension;
            }

            std::remove(tempFilePath.c_str());
        }

        shadersChanged[outputBase] = pipelineKey;
    }

    std::string FsShaderCompiler::HandleInclude(const std::filesystem::path &_file, std::unordered_set<std::filesystem::path> &_includedFiles, bool _isRoot) 
    {
        std::ifstream file(_file);
        if (!file.is_open()) {
            FsLogger::GetInstance().Log(LogType::Error, "[FsShaderCompiler] Impossible d'ouvrir le fichier du shader: " + _file.string());
        }

        std::stringstream result;
        std::string line;
        while (std::getline(file, line)) 
        {
            if (line.find("#version") != std::string::npos && !_isRoot) continue;

            if (line.find("#include") != std::string::npos) 
            {
                int first = line.find_first_of("\"");
                int last = line.find_last_of("\"");

                if (first == -1 || last == -1)
                {  
                    continue;
                }

                std::string includeContent = line.substr(first + 1, last - first - 1);

                auto file = filesMap.find(includeContent);
                if (file != filesMap.end()) 
                {
                    if (_includedFiles.find(file->second) != _includedFiles.end()) 
                    {
                        FsLogger::GetInstance().Log(LogType::Warning, "[FsShaderCompiler] Include already included: " + includeContent);
                        continue;
                    }

                    if (!IsMainShader(file->second))
                    {
                        _includedFiles.insert(file->second);
                        result << HandleInclude(file->second, _includedFiles) << '\n';
                    }
                }
                else
                {
                    FsLogger::GetInstance().Log(LogType::Warning, "[FsShaderCompiler] Include not found: " + includeContent);
                } 
            }
            else 
            {
                result << line << '\n';
            } 
        }

        return result.str();
    }

    bool FsShaderCompiler::IsMainShader(const std::filesystem::path &_file) 
    {
        std::ifstream file(_file);
        if (!file.is_open()) {
            FsLogger::GetInstance().Log(LogType::Error, "[FsShaderCompiler] Impossible d'ouvrir le fichier du shader: " + _file.string());
        }

        std::string line;
        while (std::getline(file, line)) 
        {
            if (line.find("// @Main shader") != std::string::npos) 
            {
                return true;
            }
        }

        return false;
    }
}