#pragma once 

#include "fs.window.hpp"
#include "fs.device.hpp"
#include "fs.renderer.hpp"
#include "fs.descriptors.hpp"

// std
#include <memory>
#include <string>

// mINI
#include <mini/ini.h>

namespace fs
{
    class FsApp
    {
        public:
            static constexpr int DEFAULT_WIDTH = 800;
            static constexpr int DEFAULT_HEIGHT = 600;

            FsApp(mINI::INIStructure _config);
            ~FsApp();

            FsApp(const FsWindow &) = delete;
            FsApp &operator = (const FsWindow &) = delete;

            void run();
        private:
        
        std::unique_ptr<FsWindow> window;
        std::unique_ptr<FsDevice> device;
        std::unique_ptr<FsRenderer> renderer;
        std::unique_ptr<FsDescriptorPool> globalPool{};
        
        // TODO: temporaire
        std::unique_ptr<FsDescriptorSetLayout> globalSetLayout;
        
        mINI::INIStructure config;

        void LoadGameObjects();
    };
}