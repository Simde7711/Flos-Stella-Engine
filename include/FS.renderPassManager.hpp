#pragma once

#include "FS.swapChain.hpp"

// std
#include <array>
#include <unordered_map>
#include <stdexcept>

namespace FS
{
    enum class RenderPassType 
    {
        Forward,
        Shadow,
        UI,
        GBuffer,
    };

    class RenderPassManager
    {
        public:
            RenderPassManager(LveDevice &_device);
            ~RenderPassManager();
            
            void Init(VkFormat _colorFormat, VkFormat _depthFormat, VkExtent2D _extent);
            VkRenderPass GetRenderPass(const RenderPassType _type) const;
        private:
            LveDevice &device;
            VkFormat swapChainImageFormat;
            VkFormat swapChainDepthFormat;
            VkExtent2D swapChainExtent;

            std::unordered_map<RenderPassType, VkRenderPass> renderPasses;

            void CleanupRenderPasses();
            VkRenderPass CreateForwardRenderPass();
    };
} 
