#include "systems/renderSystems/FS.meshRenderSystem.hpp"

// std
#include <stdexcept>
#include <array>
#include <cassert>

namespace lve
{
    SimpleRenderSystem::SimpleRenderSystem(LveDevice &device) : lveDevice{device}
    {

    }

    SimpleRenderSystem::~SimpleRenderSystem()
    {

    }

    void SimpleRenderSystem::RenderGameObjects(FrameInfo &frameInfo)
    {
        for (auto const &entity : gCoordinator.mEntities)
        {
            if (!gCoordinator.HasComponent<Shader>(entity)) continue;
            if (!gCoordinator.HasComponent<Model>(entity)) continue;

            auto &shaderComp = gCoordinator.GetComponent<Shader>(entity);

            LvePipeline *pipeline = shaderManager.GetPipeline(shaderComp.pipelineKey);
            pipeline->Bind(frameInfo.commandBuffer);
            vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shaderManager.GetPipelineLayout(shaderComp.pipelineKey), 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);

            Transform &transform = gCoordinator.GetComponent<Transform>(entity);

            Model &model = gCoordinator.GetComponent<Model>(entity);
            if (model.model == nullptr) continue;

            shaderComp.pushConstant.modelMatrix = transform.mat4();
            shaderComp.pushConstant.normalMatrix = transform.NormalMatrix();

            vkCmdPushConstants(frameInfo.commandBuffer, shaderManager.GetPipelineLayout(shaderComp.pipelineKey), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SinglePushConstantData), &shaderComp.pushConstant);

            model.model->Bind(frameInfo.commandBuffer);
            model.model->Draw(frameInfo.commandBuffer);
        }
    }
}