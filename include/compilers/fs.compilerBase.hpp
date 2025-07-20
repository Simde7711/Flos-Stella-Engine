#pragma once

// std
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>

namespace fs 
{
    class FsCompilerBase
    {
        public: 
            virtual void WatchForChanges(bool _startup = false) = 0;

        protected:
            std::string sourcePath;
            std::string destinationPath;
            std::unordered_map<std::string, std::filesystem::path> filesMap;
            std::unordered_map<std::filesystem::path, std::vector<std::filesystem::path>> reverseDependencyGraph;
            std::unordered_map<std::string, std::filesystem::file_time_type> filesWriteTime;
            std::vector<std::string> extensionsInput;
            std::vector<std::string> extensionsOutput;

            bool CompareFileData(const std::filesystem::path &_file);
            std::vector<std::string> ParseIncludes(const std::filesystem::path &_file);
            void GetFilesMap(); 
            virtual void Compile(const std::filesystem::path &_file) = 0;
    };
}