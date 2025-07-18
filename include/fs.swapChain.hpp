#pragma once

#include "fs.device.hpp"

// vulkan
#include <vulkan/vulkan.h>

// std
#include <vector>
#include <memory>

namespace fs 
{
  enum class RenderPassType;
  class FsRenderPassManager;

  class FsSwapChain 
  {
    public:
      static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
 
      FsSwapChain(FsDevice &deviceRef, VkExtent2D windowExtent);
      FsSwapChain(FsDevice &deviceRef, VkExtent2D windowExtent, std::shared_ptr<FsSwapChain> previous);
      ~FsSwapChain();
 
      FsSwapChain(const FsSwapChain &) = delete;
      FsSwapChain &operator=(const FsSwapChain &) = delete;
      
      VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
      
      VkRenderPass GetRenderPass(const RenderPassType _type);

      VkImageView getImageView(int index) { return swapChainImageViews[index]; }
      size_t imageCount() { return swapChainImages.size(); }
      VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
      VkExtent2D getSwapChainExtent() { return swapChainExtent; }
      uint32_t width() { return swapChainExtent.width; }
      uint32_t height() { return swapChainExtent.height; }

      float extentAspectRatio() 
      {
        return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
      }
      VkFormat findDepthFormat();

      VkResult acquireNextImage(uint32_t *imageIndex);
      VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

      bool CompareSwapFormats(const FsSwapChain &swapChain) const
      {
        return swapChain.swapChainDepthFormat == swapChainDepthFormat && 
                swapChain.swapChainImageFormat == swapChainImageFormat;
      }

    private:
      void Init();
      void createSwapChain();
      void createImageViews();
      void createDepthResources();
      void createFramebuffers();
      void createSyncObjects();

      // render pass manager
      std::unique_ptr<FsRenderPassManager> renderPassManager;

      // Helper functions
      VkSurfaceFormatKHR chooseSwapSurfaceFormat(
          const std::vector<VkSurfaceFormatKHR> &availableFormats);
      VkPresentModeKHR chooseSwapPresentMode(
          const std::vector<VkPresentModeKHR> &availablePresentModes);
      VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

      VkFormat swapChainImageFormat;
      VkFormat swapChainDepthFormat;
      VkExtent2D swapChainExtent;

      std::vector<VkFramebuffer> swapChainFramebuffers;

      std::vector<VkImage> depthImages;
      std::vector<VkDeviceMemory> depthImageMemorys;
      std::vector<VkImageView> depthImageViews;
      std::vector<VkImage> swapChainImages;
      std::vector<VkImageView> swapChainImageViews;

      FsDevice &device;
      VkExtent2D windowExtent;

      VkSwapchainKHR swapChain;
      std::shared_ptr<FsSwapChain> oldSwapChain;

      std::vector<VkSemaphore> imageAvailableSemaphores;
      std::vector<VkSemaphore> renderFinishedSemaphores;
      std::vector<VkFence> inFlightFences;
      std::vector<VkFence> imagesInFlight;
      size_t currentFrame = 0;
  };
}