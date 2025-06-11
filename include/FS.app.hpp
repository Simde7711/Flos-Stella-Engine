#pragma once 

#include "FS.window.hpp"
#include "FS.device.hpp"
#include "FS.renderer.hpp"
#include "FS.descriptors.hpp"
#include "ECS/FS.coordinator.hpp"

// std
#include <vector>
#include <memory>

namespace FS
{
    class FirstApp
    {
        public:
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            FirstApp();
            ~FirstApp();

            FirstApp(const LveWindow &) = delete;
            FirstApp &operator = (const LveWindow &) = delete;

            void run();
        private:
            void LoadGameObjects();

            LveWindow lveWindow{WIDTH, HEIGHT, "Penis Engine V0.00000000000000000000000000000000000001"};
            LveDevice lveDevice{lveWindow};
            LveRenderer lveRenderer{lveWindow, lveDevice};
            std::unique_ptr<LveDescriptorPool> globalPool{};

            // TODO: temporaire
            std::unique_ptr<LveDescriptorSetLayout> globalSetLayout;
        
    };
}