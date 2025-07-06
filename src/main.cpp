#include "fs.app.hpp"
#include "fs.logger.hpp"

// std
#include <cstdlib>

// mINI
#include <mini/ini.h>

int main()
{
    // parse le fichier de config
    mINI::INIFile file("../config.ini");
    mINI::INIStructure config;
    file.read(config);

    // logger 
    fs::FsLogger::GetInstance().Init(config["Logger"]);

    bool success = true;

    try
    {
        fs::FsApp app{config};
        app.run();
    }
    catch (const std::exception &e)
    {
        // log l'exception
        fs::FsLogger::GetInstance().Log(fs::LogType::Error, e.what());

        success = false;
    }

    // cleanup du logger
    fs::FsLogger::GetInstance().Cleanup();

    if (success)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}