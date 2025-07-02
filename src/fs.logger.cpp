#include <fs.logger.hpp>

// std
#include <iostream>
#include <chrono>

namespace fs
{
    FsLogger FsLogger::instance;

    void FsLogger::Init(const mINI::INIMap<std::string> &_logConfig)
    {
        logConfig = _logConfig;

        auto parseColor = [](const std::string& rawColor) -> std::string 
        {
            std::string result = rawColor;
            std::string::size_type pos;
            while ((pos = result.find("\\033")) != std::string::npos) 
            {
                result.replace(pos, 4, "\033");
            }
            return result;
        };

        logOption[LogType::Debug] = std::pair<bool, std::string>{(logConfig["Debug.LogInFile"] == "1"), parseColor(logConfig["Debug.LogColor"])};
        logOption[LogType::System] = std::pair<bool, std::string>{(logConfig["System.LogInFile"] == "1"), parseColor(logConfig["System.LogColor"])};
        logOption[LogType::Warning] = std::pair<bool, std::string>{(logConfig["Warning.LogInFile"] == "1"), parseColor(logConfig["Warning.LogColor"])};
        logOption[LogType::Error] = std::pair<bool, std::string>{(logConfig["Error.LogInFile"] == "1"), parseColor(logConfig["Error.LogColor"])};

        logFile.open(logConfig["logPath"]);

        Log(LogType::System, "Ouverture du logger");
    }

    void FsLogger::Cleanup()
    {
        Log(LogType::System, "Fermeture du logger");

        logFile.close();
    }

    void FsLogger::Log(LogType _logType, std::string _logMessage)
    {
        auto option = logOption.find(_logType);
        if (option != logOption.end()) 
        {
            // Ajout du temps.
            auto now = std::chrono::system_clock::now();
            std::time_t now_c = std::chrono::system_clock::to_time_t(now);

            std::stringstream formatTime;
            formatTime << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");

            // Prepare le log.
            std::string preparedLog = "[" 
                + GetTypeString(_logType) 
                + " : " 
                + formatTime.str()
                + "] " 
                + _logMessage;

            std::cout << option->second.second << preparedLog << "\033[0m\n";
            
            if (option->second.first)
            {
                LogIntoFile(preparedLog);
            }
        }
    }

    std::string FsLogger::GetTypeString(LogType _logType)
    {
        switch (_logType) {
            case LogType::Debug: return "Debug";
            case LogType::System: return "System";
            case LogType::Warning: return "Warning";
            case LogType::Error: return "Error";
            default: return "Unknown";
        }
    }

    void FsLogger::LogIntoFile(std::string _logMessage)
    {
        logFile << _logMessage << '\n';
    }

    void FsLogger::FlushFile()
    {
        logFile.flush();
    }
}
