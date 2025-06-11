#include "FS.renderer.hpp"

#include "FS.renderPassManager.hpp"
#include "FS.shaderManager.hpp"

// std
#include <stdexcept>
#include <array>
#include <cassert>

namespace FS
{
    LveRenderer::LveRenderer(LveWindow &window, LveDevice &device) : lveWindow{window}, lveDevice{device}
    {
        RecreateSwapChain();
        CreateCommandBuffers();
    }

    LveRenderer::~LveRenderer()
    {
        FreeCommandsBuffers();
    }

    void LveRenderer::RecreateSwapChain()
    {
        auto extent = lveWindow.GetExtent();
        while (extent.width == 0 || extent.height == 0)
        {
            extent = lveWindow.GetExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(lveDevice.device());

        if (lveSwapChain == nullptr)
        {
            lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent);
        }
        else
        {
            std::shared_ptr<LveSwapChain> oldSwapChain = std::move(lveSwapChain);
            lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent, oldSwapChain);

            if (!oldSwapChain->CompareSwapFormats(*lveSwapChain.get()))
            {
                throw std::runtime_error("swap chains image(or depth) format has changed");
            }

            shaderManager.RecreatePipelines();
        }
    }

    void LveRenderer::CreateCommandBuffers()
    {
        commandBuffers.resize(LveSwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo alloInfo{};
        alloInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloInfo.commandPool = lveDevice.getCommandPool();
        alloInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(lveDevice.device(), &alloInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create allocate buffers");
        }

    }

    void LveRenderer::FreeCommandsBuffers()
    {
        vkFreeCommandBuffers(
            lveDevice.device(), 
            lveDevice.getCommandPool(), 
            static_cast<uint32_t>(commandBuffers.size()), 
            commandBuffers.data()
        );
        commandBuffers.clear();
    }

    VkCommandBuffer LveRenderer::BeginFrame()
    {
        assert(!isFrameStarted && "cannot begin frame if its already started");

        auto result = lveSwapChain->acquireNextImage(&currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapChain();
            return nullptr;
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image");
        }

        isFrameStarted = true;

        auto commandBuffer = GetCurrentCommandBuffer();
        assert(commandBuffer != VK_NULL_HANDLE && "Command buffer is null!");
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer");
        }

        return commandBuffer;
    }

    void LveRenderer::EndFrame()
    {
        assert(isFrameStarted && "cannot end frame when its not started");

        auto commandBuffer = GetCurrentCommandBuffer();

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer");
        }

        auto result = lveSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || lveWindow.WasWindowResized())
        {
            lveWindow.ResetWindowResizedFlag();
            RecreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % LveSwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void LveRenderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "cannot beginSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == GetCurrentCommandBuffer() && "Cant begin render pass on command buffer from a different frame");
        
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = lveSwapChain->GetRenderPass(RenderPassType::Forward);
        renderPassInfo.framebuffer = lveSwapChain->getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = lveSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(lveSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(lveSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, lveSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void LveRenderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "cannot endSwapChainRenderPass if frame is in progress");
        assert(commandBuffer == GetCurrentCommandBuffer() && "Cant end render pass on command buffer from a same frame");
    
        vkCmdEndRenderPass(commandBuffer);
    }
}