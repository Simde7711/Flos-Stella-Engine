#include "systems/renderSystems/fs.meshRenderSystem.hpp"

// std
#include <stdexcept>
#include <array>
#include <cassert>

namespace fs
{
    FsMeshRenderSystem::FsMeshRenderSystem(FsDevice &_device) : device{_device}
    {

    }

    FsMeshRenderSystem::~FsMeshRenderSystem()
    {

    }

    void FsMeshRenderSystem::RenderGameObjects(FrameInfo &frameInfo)
    {
        for (auto const &entity : FsCoordinator::GetInstance().mEntities)
        {
            if (!FsCoordinator::GetInstance().HasComponent<Shader>(entity)) continue;
            if (!FsCoordinator::GetInstance().HasComponent<Mesh>(entity)) continue;

            auto &shaderComp = FsCoordinator::GetInstance().GetComponent<Shader>(entity);

            FsPipeline *pipeline = FsShaderManager::GetInstance().GetPipeline(shaderComp.pipelineKey);
            pipeline->Bind(frameInfo.commandBuffer);
            vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, FsShaderManager::GetInstance().GetPipelineLayout(shaderComp.pipelineKey), 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);

            Transform &transform = FsCoordinator::GetInstance().GetComponent<Transform>(entity);

            Mesh &mesh = FsCoordinator::GetInstance().GetComponent<Mesh>(entity);
            if (mesh.model == nullptr) continue;

            shaderComp.pushConstant.modelMatrix = transform.mat4();
            shaderComp.pushConstant.normalMatrix = transform.NormalMatrix();

            vkCmdPushConstants(frameInfo.commandBuffer, FsShaderManager::GetInstance().GetPipelineLayout(shaderComp.pipelineKey), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SinglePushConstantData), &shaderComp.pushConstant);

            mesh.model->Bind(frameInfo.commandBuffer);
            mesh.model->Draw(frameInfo.commandBuffer);
        }
    }
}