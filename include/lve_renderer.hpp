#pragma once 

#include "lve_window.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"

// std
#include <cassert>
#include <vector>
#include <memory>

namespace lve
{
    class LveRenderer
    {
        public:

            LveRenderer(LveWindow &window, LveDevice &device);
            ~LveRenderer();

            LveRenderer(const LveWindow &) = delete;
            LveRenderer &operator = (const LveWindow &) = delete;

            bool IsFrameInProgress() const { return isFrameStarted; }
            float GetAspectRatio() const { return lveSwapChain->extentAspectRatio(); }
            VkRenderPass GetSwapChainRenderPass() const { return lveSwapChain->getRenderPass(); }

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

            LveWindow &lveWindow;
            LveDevice &lveDevice;
            std::unique_ptr<LveSwapChain> lveSwapChain;
            std::vector<VkCommandBuffer> commandBuffers;

            uint32_t currentImageIndex;
            int currentFrameIndex{0};
            bool isFrameStarted;
    };
}