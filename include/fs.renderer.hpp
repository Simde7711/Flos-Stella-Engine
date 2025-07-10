#pragma once 

#include "fs.window.hpp"
#include "fs.device.hpp"
#include "fs.swapChain.hpp"

#include <vulkan/vulkan.h>

// std
#include <cassert>
#include <vector>
#include <memory>

namespace fs
{
    class FsRenderer
    {
        public:

            FsRenderer(FsWindow &window, FsDevice &device);
            ~FsRenderer();

            FsRenderer(const FsWindow &) = delete;
            FsRenderer &operator = (const FsWindow &) = delete;

            bool IsFrameInProgress() const { return isFrameStarted; }
            float GetAspectRatio() const { return swapChain->extentAspectRatio(); }
            VkRenderPass GetSwapChainRenderPass(const RenderPassType _type) const { return swapChain->GetRenderPass(_type); }

            VkCommandBuffer GetCurrentCommandBuffer() const 
            {
                assert(isFrameStarted && "Cannot get command buffer when frame is not in progress");
                return commandBuffers[currentFrameIndex]; 
            }

            int GetFrameIndex() const
            {
                assert(isFrameStarted && "Cannot get frame index when frame is not in progress");
                return currentFrameIndex;
            }

            VkCommandBuffer BeginFrame();
            void EndFrame();
            void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
            void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

        private:
            void CreateCommandBuffers();
            void FreeCommandsBuffers();
            void RecreateSwapChain();

            FsWindow &window;
            FsDevice &device;
            std::unique_ptr<FsSwapChain> swapChain;
            std::vector<VkCommandBuffer> commandBuffers;

            uint32_t currentImageIndex;
            int currentFrameIndex{0};
            bool isFrameStarted;
    };
}
