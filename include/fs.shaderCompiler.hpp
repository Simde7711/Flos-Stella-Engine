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
    /// @brief Classe pour la compilation de shader durant l'éxecution de l'application.
    /// @details Cette classe permet de recompilation les shaders qui ont été changer durant l'éxecution. 
    ///     La recréation des pipelines concernés est gérés.
    class FsShaderCompiler
    {
        public:
            /// @brief Initiation de la classe ShaderCompiler
            /// @param _device (VkDevice) Device pour Vulkan.
            /// @param _sourcePath (std::string) Chemin du dossier contenant les fichiers de shader.
            /// @param _destinationPath (std::string) Chemin du dossier qui va contenir les shaders compilés en .spv.
            void Init(VkDevice _device, std::string _sourcePath, std::string _destinationPath);
            
            /// @brief Regarde pour les changements dans les fichers de shader
            /// @param _startup (bool) Empêche de recréer des pipelines qu'on pas encore été créés (Défaut à false).
            void WatchForChanges(bool _startup = false);
            
            /// @brief Compare le temps de modifaction du fichier de shaders à celle dans la cache.
            /// @param _file (std::filesystem::__cxx11::path) Chemin du ficher à comparer dans la cache.
            /// @return (bool) Retourne true si le le temps de modification n'est pas la même que la celle dans le cache, sinon false.
            bool CompareFileData(const std::filesystem::__cxx11::path &_file);
            
            /// @brief S'occupe de la compilation des fichers de shader vers .spv.
            /// @param _file (std::filesystem::__cxx11::path) Chemin du ficher à recompiler.
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
