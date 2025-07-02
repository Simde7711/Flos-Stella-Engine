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

namespace fs
{
    // unique instance de shaderManager
    FsShaderManager FsShaderManager::instance;

    void FsShaderManager::Init(FsDevice *_device, FsRenderer *_renderer, VkDescriptorSetLayout _globalSetLayout)
    {   
        device = _device;
        renderer = _renderer;
        globalSetLayout = _globalSetLayout;
    }

    const PipelineKey FsShaderManager::GetOrCreatePipelineKey(const PipelineKey &_key)
    {
        auto it = pipelineCache.find(_key);
        if (it != pipelineCache.end()) 
        {
            return it->first;
        }

        // Create config
        PipelineConfigInfo configInfo{};
        FsPipeline::defaultPipelineConfigInfo(configInfo);
        
        configInfo.pipelineLayout = GetPipelineLayoutForKey(_key);
        configInfo.renderPass = renderer->GetSwapChainRenderPass(_key.config.renderPassType);

        auto pipeline = std::make_unique<FsPipeline>
        (
            *device, 
            _key.vertShaderPath, 
            _key.fragShaderPath, 
            configInfo
        );

        pipelineCache[_key] = { std::move(pipeline), configInfo.pipelineLayout };

        return _key;
    }

    void FsShaderManager::RecreatePipelinesFromShaderPath(const PipelineKey &_key)
    {
        for (auto& [key, data] : pipelineCache)
        {
            if (key.vertShaderPath == _key.vertShaderPath && key.fragShaderPath == _key.fragShaderPath)
            {
                vkDestroyPipelineLayout(device->device(), data.pipelineLayout, nullptr);

                PipelineConfigInfo configInfo{};
                FsPipeline::defaultPipelineConfigInfo(configInfo);
                configInfo.pipelineLayout = GetPipelineLayoutForKey(key);
                configInfo.renderPass = renderer->GetSwapChainRenderPass(key.config.renderPassType);

                auto newPipeline = std::make_unique<FsPipeline>
                (
                    *device,
                    key.vertShaderPath,
                    key.fragShaderPath,
                    configInfo
                );

                pipelineCache[key] = { std::move(newPipeline), configInfo.pipelineLayout };
            }
        }
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

    // TODO: faire en que sa fait le caching du layout (const PipelineLayoutKey FsShaderManager::GetOrCreatePipelineLayoutKey(const PipelineLayoutKey &_key))
    VkPipelineLayout FsShaderManager::GetPipelineLayoutForKey(const PipelineKey &_key) 
    {
        // TODO NE PAS FAIRE SA, cache les descriptors avec la création du key auparavant qui les contients
        std::vector<VkDescriptorSetLayout> descriptorsLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorsLayouts.size());
        layoutInfo.pSetLayouts = descriptorsLayouts.data();

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0; 
        pushConstantRange.size = sizeof(SinglePushConstantData);

        layoutInfo.pushConstantRangeCount = 1;
        layoutInfo.pPushConstantRanges = &pushConstantRange;

        VkPipelineLayout layout;
        if (vkCreatePipelineLayout(device->device(), &layoutInfo, nullptr, &layout) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create pipeline layout");
        }
        return layout;
    }

    FsPipeline* FsShaderManager::GetPipeline(const PipelineKey &_key)
    {
        auto it = pipelineCache.find(_key);

        if (it == pipelineCache.end())
        {
            throw std::runtime_error("Pipeline not found in cache");
        }
        return it->second.pipeline.get();
    }

    VkPipelineLayout FsShaderManager::GetPipelineLayout(const PipelineKey &_key)
    {
        auto it = pipelineCache.find(_key);

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