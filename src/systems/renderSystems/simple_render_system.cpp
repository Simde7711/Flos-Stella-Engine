#include "systems/renderSystems/simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm.hpp"
#include "gtc/constants.hpp"

// std
#include <stdexcept>
#include <array>
#include <cassert>

namespace lve
{
    struct SinglePushConstantData{
        glm::mat4 modelMatrix{1.f};
        glm::mat4 normalMatrix{1.f};
    };

    SimpleRenderSystem::SimpleRenderSystem(LveDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : lveDevice{device}
    {
        CreatePipelineLayout(globalSetLayout);
        CreatePipeline(renderPass);
    }

    SimpleRenderSystem::~SimpleRenderSystem()
    {
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
    }

    void SimpleRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0; 
        pushConstantRange.size = sizeof(SinglePushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayout{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayout.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayout.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        
        if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void SimpleRenderSystem::CreatePipeline(VkRenderPass renderPass)
    {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<LvePipeline>(
            lveDevice, 
            "../shaders/simple_shader.vert.spv", 
            "../shaders/simple_shader.frag.spv", 
            pipelineConfig
        );
    }

    void SimpleRenderSystem::RenderGameObjects(FrameInfo &frameInfo)
    {
        lvePipeline->Bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);

        for (auto const &entity : gCoordinator.mEntities)
        {
            Transform &transform = gCoordinator.GetComponent<Transform>(entity);

            if (!gCoordinator.HasComponent<Model>(entity)) continue;

            Model &model = gCoordinator.GetComponent<Model>(entity);
            if (model.model == nullptr) continue;

            SinglePushConstantData push{};
            push.modelMatrix = transform.mat4();
            push.normalMatrix = transform.NormalMatrix();

            vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SinglePushConstantData), &push);

            model.model->Bind(frameInfo.commandBuffer);
            model.model->Draw(frameInfo.commandBuffer);
        }
    }
}