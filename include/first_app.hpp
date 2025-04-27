#pragma once 

#include "lve_window.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"

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
            void CreatePipelineLayout();
            void CreatePipeline();
            void CreateCommandBuffers();
            void DrawFrame();

            LveWindow lveWindow{WIDTH, HEIGHT, "Hello World"};
            LveDevice lveDevice{lveWindow};
            
            LveSwapChain lveSwapChain{lveDevice, lveWindow.GetExtent()};
            std::unique_ptr<LvePipeline> lvePipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
    };
}