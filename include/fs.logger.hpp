#pragma once

// std
#include <string>
#include <fstream>
#include <unordered_map>
#include <utility>

// mINI
#include "ini.h"

namespace fs
{
    /// @brief Les types de log.
    enum class LogType
    {
        Debug,
        System,
        Warning,
        Error
    };

    /// @brief 
    class FsLogger
    {
        public:
            static FsLogger &GetInstance()
            {
                return instance;
            }

            /// @brief Initiation de la classe logger.
            /// @param _logConfig 
            void Init(const mINI::INIMap<std::string> &_logConfig);

            /// @brief S'occupe de la fermeture du logger.
            void Cleanup();

            /// @brief Log selon un type de log et le message envoyé.
            /// @param _logType 
            /// @param _logMessage 
            void Log(LogType _logType, std::string _logMessage);

            /// @brief Flush le buffer dans le fichier pour l'écriture.
            void FlushFile();
        private:
            static FsLogger instance;
            mINI::INIMap<std::string> logConfig;
            std::unordered_map<LogType, std::pair<bool, std::string>> logOption;
            std::ofstream logFile;

            void LogIntoFile(std::string _logMessage);
            std::string GetTypeString(LogType _logType);
    };
}
