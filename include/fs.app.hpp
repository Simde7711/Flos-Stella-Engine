#pragma once 

#include "fs.window.hpp"
#include "fs.device.hpp"
#include "fs.renderer.hpp"
#include "fs.descriptors.hpp"
#include "ECS/fs.coordinator.hpp"

// std
#include <vector>
#include <memory>

namespace FS
{
    class FsApp
    {
        public:
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            FsApp();
            ~FsApp();

            FsApp(const FsWindow &) = delete;
            FsApp &operator = (const FsWindow &) = delete;

            void run();
        private:
            void LoadGameObjects();

            FsWindow window{WIDTH, HEIGHT, "Flōs Stella Engine V0.002"};
            FsDevice device{window};
            FsRenderer renderer{window, device};
            std::unique_ptr<FsDescriptorPool> globalPool{};

            // TODO: temporaire
            std::unique_ptr<FsDescriptorSetLayout> globalSetLayout;
        
    };
}