#include "First_app.hpp"

// std
#include <stdexcept>

namespace lve
{
    FirstApp::FirstApp()
    {
        CreatePipelineLayout();
        CreatePipeline();
        CreateCommandBuffers();
    }

    FirstApp::~FirstApp()
    {
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
    }

    void FirstApp::run()
    {
        while (!lveWindow.ShouldClose())
        {
            glfwPollEvents();
        }
    }

    void FirstApp::CreatePipelineLayout()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        
        if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void FirstApp::CreatePipeline()
    {
        PipelineConfigInfo pipelineConfig = LvePipeline::defaultPipelineConfigInfo(lveSwapChain.width(), lveSwapChain.height());
        pipelineConfig.renderPass = lveSwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<LvePipeline>(
            lveDevice, 
            "../shaders/simple_shader.vert.spv", 
            "../shaders/simple_shader.frag.spv", 
            pipelineConfig
        );
    }

    void FirstApp::CreateCommandBuffers()
    {

    }

    void FirstApp::DrawFrame()
    {

    }
}