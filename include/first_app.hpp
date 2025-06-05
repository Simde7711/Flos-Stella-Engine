#pragma once 

#include "lve_window.hpp"
#include "lve_device.hpp"
#include "lve_renderer.hpp"
#include "lve_descriptors.hpp"
#include "ECS/coordinator.hpp"

// std
#include <vector>
#include <memory>

namespace lve
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
    };
}