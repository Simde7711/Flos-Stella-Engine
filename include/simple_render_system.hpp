#pragma once 

#include "lve_camera.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "lve_frame_info.hpp"


#include <vector>
#include <memory>

namespace lve
{
    class SimpleRenderSystem
    {
        public:
            SimpleRenderSystem(LveDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
            ~SimpleRenderSystem();

            SimpleRenderSystem(const LveWindow &) = delete;
            SimpleRenderSystem &operator = (const LveWindow &) = delete;

            void RenderGameObjects(FrameInfo &frameInfo);
        private:
            void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
            void CreatePipeline(VkRenderPass renderPass);

            LveDevice &lveDevice;

            std::unique_ptr<LvePipeline> lvePipeline;
            VkPipelineLayout pipelineLayout;
    };
}