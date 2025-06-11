#pragma once

#include "FS.device.hpp"

// std
#include <string>
#include <vector>

namespace FS
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

        // bool operator==(const PipelineConfigInfo& other) const 
        // {
        //     return inputAssemblyInfo.topology == other.inputAssemblyInfo.topology &&
        //         rasterizationInfo.cullMode == other.rasterizationInfo.cullMode &&
        //         rasterizationInfo.polygonMode == other.rasterizationInfo.polygonMode &&
        //         multisampleInfo.rasterizationSamples == other.multisampleInfo.rasterizationSamples &&
        //         colorBlendAttachment.blendEnable == other.colorBlendAttachment.blendEnable &&
        //         depthStencilInfo.depthTestEnable == other.depthStencilInfo.depthTestEnable &&
        //         depthStencilInfo.depthWriteEnable == other.depthStencilInfo.depthWriteEnable &&
        //         pipelineLayout == other.pipelineLayout &&
        //         renderPass == other.renderPass &&
        //         subpass == other.subpass;
        // }
    };

    class LvePipeline
    {
        public:
            LvePipeline(LveDevice &device, const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo &configInfo);
            ~LvePipeline();

            LvePipeline(const LvePipeline &) = delete;
            LvePipeline &operator=(const LvePipeline &) = delete;

            void Bind(VkCommandBuffer commandBuffer);
            static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);
            static void EnableAlphaBlending(PipelineConfigInfo &configInfo);

        private:
            static std::vector<char> ReadFile(const std::string& filepath);
            void CreateGraphicPipeline(const std::string &vertFilepath, const std::string &fragFilepath, const PipelineConfigInfo &configInfo);
    
            void CreateShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

            LveDevice &lveDevice;

            VkPipeline graphicPipeline;
            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;
    };
}