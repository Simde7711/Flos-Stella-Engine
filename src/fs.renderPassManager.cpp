#include "fs.renderPassManager.hpp"
#include "fs.device.hpp"

#include <vulkan/vulkan.h>

// std
#include <array> 
#include <stdexcept>

namespace fs
{
    FsRenderPassManager::FsRenderPassManager(FsDevice &_device) : device{_device}
    {

    }

    FsRenderPassManager::~FsRenderPassManager()
    {
        CleanupRenderPasses();
    }

    void FsRenderPassManager::Init(VkFormat _colorFormat, VkFormat _depthFormat, VkExtent2D _extent)
    {
        swapChainImageFormat = _colorFormat;
        swapChainDepthFormat = _depthFormat;
        swapChainExtent = _extent;

        renderPasses[RenderPassType::Forward] = CreateForwardRenderPass();
        // renderPasses[RenderPassType::Shadow] = CreateShadowRenderPass();
    }

    VkRenderPass FsRenderPassManager::GetRenderPass(const RenderPassType _type) const 
    {
        return renderPasses.at(_type);
    }


    void FsRenderPassManager::CleanupRenderPasses()
    {
        for (auto& [_, renderPass] : renderPasses) 
        {
            if (renderPass != VK_NULL_HANDLE) 
            {
                vkDestroyRenderPass(device.device(), renderPass, nullptr);
            }
        }
        renderPasses.clear();
    }

    VkRenderPass FsRenderPassManager::CreateForwardRenderPass() 
    {
        VkRenderPass forwardRenderPass{};

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = swapChainDepthFormat;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.srcAccessMask = 0;
        dependency.srcStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstSubpass = 0;
        dependency.dstStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask =
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(device.device(), &renderPassInfo, nullptr, &forwardRenderPass) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create render pass!");
        }

        return forwardRenderPass;
    }
}
