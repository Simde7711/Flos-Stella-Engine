#include "fs.shaderManager.hpp"

// std
#include <iostream>

// void DebugCacheContenu(auto &cache)
// {
//     int nombre{};
//     for (auto itr = cache.begin(); itr != cache.end(); itr++) 
//     {   
//         nombre++;
//     }

//     std::cout << "Le cache de shader contient: " << nombre << '\n';
// }

namespace FS
{
    // unique instance de shaderManager
    FsShaderManager shaderManager;

    void FsShaderManager::Init(FsDevice *_device, FsRenderer *_renderer, VkDescriptorSetLayout _globalSetLayout)
    {   
        device = _device;
        renderer = _renderer;
        globalSetLayout = _globalSetLayout;
    }

    const PipelineKey FsShaderManager::GetOrCreatePipelineKey(const PipelineKey& key)
    {
        auto it = pipelineCache.find(key);
        if (it != pipelineCache.end()) 
        {
            return it->first;
        }

        // Create config
        PipelineConfigInfo configInfo{};
        FsPipeline::defaultPipelineConfigInfo(configInfo);
        
        configInfo.pipelineLayout = GetPipelineLayoutForKey(key);
        configInfo.renderPass = renderer->GetSwapChainRenderPass(key.config.renderPassType);

        auto pipeline = std::make_unique<FsPipeline>
        (
            *device, 
            key.vertShaderPath, 
            key.fragShaderPath, 
            configInfo
        );

        pipelineCache[key] = { std::move(pipeline), configInfo.pipelineLayout };

        return key;
    }

    void FsShaderManager::RecreatePipelines()
    {
        std::unordered_map<PipelineKey, PipelineData, PipelineKeyHasher> newCache;

        for (auto& [oldKey, oldData] : pipelineCache)
        {   
            vkDestroyPipelineLayout(device->device(), oldData.pipelineLayout, nullptr);

            // Nouveau key avec le nouveau renderPass
            PipelineKey newKey = oldKey;

            PipelineConfigInfo configInfo{};
            FsPipeline::defaultPipelineConfigInfo(configInfo);
            configInfo.pipelineLayout = GetPipelineLayoutForKey(newKey);
            configInfo.renderPass = renderer->GetSwapChainRenderPass(newKey.config.renderPassType);

            auto newPipeline = std::make_unique<FsPipeline>
            (
                *device,
                newKey.vertShaderPath,
                newKey.fragShaderPath,
                configInfo
            );

            newCache[newKey] = { std::move(newPipeline), configInfo.pipelineLayout };
        }

        // Remplace l'ancien
        pipelineCache = std::move(newCache);
    }

    VkPipelineLayout FsShaderManager::GetPipelineLayoutForKey(const PipelineKey& key) 
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0; 
        pushConstantRange.size = sizeof(SinglePushConstantData);

        // TODO NE PAS FAIRE SA, cache les descriptors
        std::vector<VkDescriptorSetLayout> descriptorsLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorsLayouts.size());
        layoutInfo.pSetLayouts = descriptorsLayouts.data();
        layoutInfo.pushConstantRangeCount = 1;
        layoutInfo.pPushConstantRanges = &pushConstantRange;

        VkPipelineLayout layout;
        if (vkCreatePipelineLayout(device->device(), &layoutInfo, nullptr, &layout) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create pipeline layout");
        }
        return layout;
    }

    FsPipeline* FsShaderManager::GetPipeline(const PipelineKey& key)
    {
        auto it = pipelineCache.find(key);

        if (it == pipelineCache.end())
        {
            throw std::runtime_error("Pipeline not found in cache");
        }
        return it->second.pipeline.get();
    }

    VkPipelineLayout FsShaderManager::GetPipelineLayout(const PipelineKey& key)
    {
        auto it = pipelineCache.find(key);

        if (it == pipelineCache.end())
        {
            throw std::runtime_error("PipelineLayout not found in cache");
        }
        return it->second.pipelineLayout;
    }

    void FsShaderManager::Cleanup()
    {
        for (auto& [key, data] : pipelineCache)
        {
            if (data.pipelineLayout != VK_NULL_HANDLE)
            {
                vkDestroyPipelineLayout(device->device(), data.pipelineLayout, nullptr);
            }
        }
        
        device = nullptr;
        renderer = nullptr;
        pipelineCache.clear();
    }
}