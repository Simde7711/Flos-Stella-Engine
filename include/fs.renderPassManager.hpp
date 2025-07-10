#pragma once

#include "fs.device.hpp"

#include <vulkan/vulkan.h>

// std
#include <unordered_map>

namespace fs
{
    enum class RenderPassType 
    {
        Forward,
        Shadow,
        UI,
        GBuffer,
    };

    class FsRenderPassManager
    {
        public:
            FsRenderPassManager(FsDevice &_device);
            ~FsRenderPassManager();
            
            void Init(VkFormat _colorFormat, VkFormat _depthFormat, VkExtent2D _extent);
            VkRenderPass GetRenderPass(const RenderPassType _type) const;
        private:
            FsDevice &device;
            VkFormat swapChainImageFormat;
            VkFormat swapChainDepthFormat;
            VkExtent2D swapChainExtent;

            std::unordered_map<RenderPassType, VkRenderPass> renderPasses;

            void CleanupRenderPasses();
            VkRenderPass CreateForwardRenderPass();
    };
} 
