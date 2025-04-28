#pragma once 

#include "lve_window.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"
#include "lve_game_object.hpp"


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
            void CreatePipelineLayout();
            void CreatePipeline();
            void CreateCommandBuffers();
            void FreeCommandsBuffers();
            void DrawFrame();
            void RecreateSwapChain();
            void RecordCommandBuffer(int imageIndex);
            void RenderGameObjects(VkCommandBuffer commandBuffer);

            LveWindow lveWindow{WIDTH, HEIGHT, "Penis Engine V0.0000000000000000000000000000000000000000000000000000000000000000001"};
            LveDevice lveDevice{lveWindow};
            std::unique_ptr<LveSwapChain> lveSwapChain;
            std::unique_ptr<LvePipeline> lvePipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
            std::vector<LveGameObject> gameObjects;
    };
}