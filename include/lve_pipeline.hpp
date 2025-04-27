#pragma once

#include "lve_device.hpp"

// std
#include <string>
#include <vector>

namespace lve
{
    struct PipelineConfigInfo {
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

    class LvePipeline
    {
        public:
            LvePipeline(LveDevice &device, const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo &configInfo);
            ~LvePipeline();

            LvePipeline(const LvePipeline &) = delete;
            LvePipeline &operator=(const LvePipeline &) = delete;

            void Bind(VkCommandBuffer commandBuffer);
            static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);

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