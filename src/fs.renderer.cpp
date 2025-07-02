#include "fs.renderer.hpp"

#include "fs.renderPassManager.hpp"
#include "fs.shaderManager.hpp"

// std
#include <stdexcept>
#include <array>
#include <cassert>
#include <iostream>

namespace fs
{
    FsRenderer::FsRenderer(FsWindow &window, FsDevice &device) : window{window}, device{device}
    {
        RecreateSwapChain();
        CreateCommandBuffers();
    }

    FsRenderer::~FsRenderer()
    {
        FreeCommandsBuffers();
    }

    void FsRenderer::RecreateSwapChain()
    {
        auto extent = window.GetExtent();
        while (extent.width == 0 || extent.height == 0)
        {
            extent = window.GetExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(device.device());

        if (swapChain == nullptr)
        {
            swapChain = std::make_unique<FsSwapChain>(device, extent);
        }
        else
        {
            std::shared_ptr<FsSwapChain> oldSwapChain = std::move(swapChain);
            swapChain = std::make_unique<FsSwapChain>(device, extent, oldSwapChain);

            if (!oldSwapChain->CompareSwapFormats(*swapChain.get()))
            {
                throw std::runtime_error("swap chains image(or depth) format has changed");
            }

            std::cout << "[FsRenderer] Recreation des pipelines a cause d'un changement dans la swapChain." << '\n';
            FsShaderManager::GetInstance().RecreatePipelines();
        }
    }

    void FsRenderer::CreateCommandBuffers()
    {
        commandBuffers.resize(FsSwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo alloInfo{};
        alloInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloInfo.commandPool = device.getCommandPool();
        alloInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(device.device(), &alloInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create allocate buffers");
        }

    }

    void FsRenderer::FreeCommandsBuffers()
    {
        vkFreeCommandBuffers(
            device.device(), 
            device.getCommandPool(), 
            static_cast<uint32_t>(commandBuffers.size()), 
            commandBuffers.data()
        );
        commandBuffers.clear();
    }

    VkCommandBuffer FsRenderer::BeginFrame()
    {
        assert(!isFrameStarted && "cannot begin frame if its already started");

        auto result = swapChain->acquireNextImage(&currentImageIndex);

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

    void FsRenderer::EndFrame()
    {
        assert(isFrameStarted && "cannot end frame when its not started");

        auto commandBuffer = GetCurrentCommandBuffer();

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer");
        }

        auto result = swapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.WasWindowResized())
        {
            window.ResetWindowResizedFlag();
            RecreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % FsSwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void FsRenderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "cannot beginSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == GetCurrentCommandBuffer() && "Cant begin render pass on command buffer from a different frame");
        
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChain->GetRenderPass(RenderPassType::Forward);
        renderPassInfo.framebuffer = swapChain->getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, swapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void FsRenderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "cannot endSwapChainRenderPass if frame is in progress");
        assert(commandBuffer == GetCurrentCommandBuffer() && "Cant end render pass on command buffer from a same frame");
    
        vkCmdEndRenderPass(commandBuffer);
    }
}