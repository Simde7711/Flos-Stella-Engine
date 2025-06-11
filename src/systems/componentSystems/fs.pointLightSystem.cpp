#include "systems/componentSystems/fs.pointLightSystem.hpp"

namespace fs
{
    void FsPointLightSystem::Update(FrameInfo &frameInfo, GlobalUbo &globalUbo)
    {
        auto rotateLight =  
                glm::rotate(glm::mat4(1.f), 
                frameInfo.frameTime, 
                {0.f, -1.f, 0.f});

        int lightIndex = 0;

        for (auto const &entity : gCoordinator.mEntities)
        {
            if (!gCoordinator.HasComponent<PointLight>(entity))
            {
                continue; // skip entities missing required components
            }

            
            // update light position
            Transform &transform = gCoordinator.GetComponent<Transform>(entity);
            transform.translation = glm::vec3(rotateLight * glm::vec4(transform.translation, 1.f));

            // copy light to ubo
            PointLight &pointLight = gCoordinator.GetComponent<PointLight>(entity);
            globalUbo.pointLights[lightIndex].position = glm::vec4(transform.translation, 1.f);
            globalUbo.pointLights[lightIndex].color = glm::vec4(pointLight.color, pointLight.lightIntensity);
            
            lightIndex++;
        }

        globalUbo.numLights = lightIndex;
    } 
}
