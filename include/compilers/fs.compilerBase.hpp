#pragma once

// std
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>

// nlohmann
#include <nlohmann/json.hpp>

namespace fs 
{
    class FsCompilerBase
    {
        public: 
            FsCompilerBase(nlohmann::json &_timeCache);

            virtual void WatchForChanges(bool _startup = false) = 0;

        protected:
            std::string sourcePath;
            std::string destinationPath;
            std::unordered_map<std::string, std::filesystem::path> filesMap;
            std::unordered_map<std::filesystem::path, std::vector<std::filesystem::path>> reverseDependencyGraph;
            std::unordered_map<std::string, std::filesystem::file_time_type> filesWriteTime;
            std::vector<std::string> extensionsInput;
            std::vector<std::string> extensionsOutput;

            nlohmann::json &timeCache;

            bool CompareFileData(const std::filesystem::path &_file, bool _giveOutput);
            std::vector<std::string> ParseIncludes(const std::filesystem::path &_file);
            void GetFilesMap();
            void GetReverseDependencyGraph(std::vector<std::string> _excludes = {});
            virtual void Compile(const std::filesystem::path &_file) = 0;
    };
}