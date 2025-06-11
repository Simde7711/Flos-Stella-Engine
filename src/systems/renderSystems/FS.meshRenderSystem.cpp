#include "systems/renderSystems/FS.meshRenderSystem.hpp"

// std
#include <stdexcept>
#include <array>
#include <cassert>

namespace FS
{
    MeshRenderSystem::MeshRenderSystem(LveDevice &device) : lveDevice{device}
    {

    }

    MeshRenderSystem::~MeshRenderSystem()
    {

    }

    void MeshRenderSystem::RenderGameObjects(FrameInfo &frameInfo)
    {
        for (auto const &entity : gCoordinator.mEntities)
        {
            if (!gCoordinator.HasComponent<Shader>(entity)) continue;
            if (!gCoordinator.HasComponent<Mesh>(entity)) continue;

            auto &shaderComp = gCoordinator.GetComponent<Shader>(entity);

            LvePipeline *pipeline = shaderManager.GetPipeline(shaderComp.pipelineKey);
            pipeline->Bind(frameInfo.commandBuffer);
            vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shaderManager.GetPipelineLayout(shaderComp.pipelineKey), 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);

            Transform &transform = gCoordinator.GetComponent<Transform>(entity);

            Mesh &mesh = gCoordinator.GetComponent<Mesh>(entity);
            if (mesh.model == nullptr) continue;

            shaderComp.pushConstant.modelMatrix = transform.mat4();
            shaderComp.pushConstant.normalMatrix = transform.NormalMatrix();

            vkCmdPushConstants(frameInfo.commandBuffer, shaderManager.GetPipelineLayout(shaderComp.pipelineKey), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SinglePushConstantData), &shaderComp.pushConstant);

            mesh.model->Bind(frameInfo.commandBuffer);
            mesh.model->Draw(frameInfo.commandBuffer);
        }
    }
}