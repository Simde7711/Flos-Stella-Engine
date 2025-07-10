#pragma once

#include "fs.camera.hpp"

#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace fs
{
    #define MAX_LIGHTS 10
    
    struct PointLightTemporaryToChangeSoon
    {
        glm::vec4 position{}; // ignore w
        glm::vec4 color{}; // w is intensity

    };

    struct GlobalUbo
    {
        glm::mat4 projection{1.f};
        glm::mat4 view{1.f};
        glm::mat4 inverseView{1.f};
        glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f}; // w is intensity
        PointLightTemporaryToChangeSoon pointLights[MAX_LIGHTS];
        int numLights;
    };

    struct FrameInfo
    {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        FsCamera &camera;
        VkDescriptorSet globalDescriptorSet;
    };
}