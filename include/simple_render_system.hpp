#pragma once 

#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_game_object.hpp"


#include <vector>
#include <memory>

namespace lve
{
    class SimpleRenderSystem
    {
        public:
            SimpleRenderSystem(LveDevice &device, VkRenderPass renderPass);
            ~SimpleRenderSystem();

            SimpleRenderSystem(const LveWindow &) = delete;
            SimpleRenderSystem &operator = (const LveWindow &) = delete;

            void RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<LveGameObject> &gameObjects);
        private:
            void CreatePipelineLayout();
            void CreatePipeline(VkRenderPass renderPass);

            LveDevice &lveDevice;

            std::unique_ptr<LvePipeline> lvePipeline;
            VkPipelineLayout pipelineLayout;
    };
}