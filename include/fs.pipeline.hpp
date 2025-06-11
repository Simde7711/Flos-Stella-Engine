#pragma once

#include "fs.device.hpp"

// std
#include <string>
#include <vector>

namespace fs
{
    struct PipelineConfigInfo 
    {
        PipelineConfigInfo() = default;
        PipelineConfigInfo(const PipelineConfigInfo &) = delete;
        PipelineConfigInfo &operator=(const PipelineConfigInfo &) = delete;

        std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class FsPipeline
    {
        public:
            FsPipeline(FsDevice &device, const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo &configInfo);
            ~FsPipeline();

            FsPipeline(const FsPipeline &) = delete;
            FsPipeline &operator=(const FsPipeline &) = delete;

            void Bind(VkCommandBuffer commandBuffer);
            static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);
            static void EnableAlphaBlending(PipelineConfigInfo &configInfo);

        private:
            static std::vector<char> ReadFile(const std::string& filepath);
            void CreateGraphicPipeline(const std::string &vertFilepath, const std::string &fragFilepath, const PipelineConfigInfo &configInfo);
    
            void CreateShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

            FsDevice &device;

            VkPipeline graphicPipeline;
            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;
    };
}