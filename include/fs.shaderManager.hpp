#pragma once

#include "fs.device.hpp"
#include "fs.pipeline.hpp"
#include "fs.renderPassManager.hpp"
#include "fs.renderer.hpp"

#include <vulkan/vulkan.h>

// glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <unordered_map>
#include <string>
#include <functional>

namespace fs
{
    struct SinglePushConstantData
    {
        glm::mat4 modelMatrix{1.f};
        glm::mat4 normalMatrix{1.f};
    };

    struct PipelineConfigKey 
    {
        RenderPassType renderPassType = RenderPassType::Forward;
        bool alphaBlendingEnabled = false;
        VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
        bool depthTestEnable = true;
        bool depthWriteEnable = true;
        VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS;

        bool operator==(const PipelineConfigKey& other) const 
        {
            return renderPassType == other.renderPassType &&
                alphaBlendingEnabled == other.alphaBlendingEnabled &&
                cullMode == other.cullMode &&
                depthTestEnable == other.depthTestEnable &&
                depthWriteEnable == other.depthWriteEnable &&
                depthCompareOp == other.depthCompareOp;
        }
    };

    struct PipelineConfigKeyHasher 
    {
        std::size_t operator()(const PipelineConfigKey& key) const 
        {
            size_t h = std::hash<int>()(static_cast<int>(key.renderPassType));
            h ^= std::hash<bool>()(key.alphaBlendingEnabled) << 1;
            h ^= std::hash<uint32_t>()(key.cullMode) << 2;
            h ^= std::hash<bool>()(key.depthTestEnable) << 3;
            h ^= std::hash<bool>()(key.depthWriteEnable) << 4;
            h ^= std::hash<int>()(static_cast<int>(key.depthCompareOp)) << 5;
            return h;
        }
    };

    struct PipelineKey 
    {
        std::string vertShaderPath;
        std::string fragShaderPath;
        PipelineConfigKey config;

        bool operator==(const PipelineKey& other) const 
        {
            return vertShaderPath == other.vertShaderPath &&
                fragShaderPath == other.fragShaderPath &&
                config == other.config;
        }
    };

    struct PipelineKeyHasher 
    {
        std::size_t operator()(const PipelineKey& k) const 
        {
            size_t h = std::hash<std::string>()(k.vertShaderPath);
            h ^= std::hash<std::string>()(k.fragShaderPath) << 1;
            h ^= PipelineConfigKeyHasher{}(k.config) << 2;
            return h;
        }
    };

    // TODO: à utilisé pour le caching, va contenir pushConstants et descriptorSets
    struct PipelineLayoutKey
    {

    };

    class FsShaderManager
    {
        public:
            // TODO: layoutData va contenir le layout
            struct PipelineData 
            {
                std::unique_ptr<FsPipeline> pipeline;
                VkPipelineLayout pipelineLayout;
            };

            static FsShaderManager &GetInstance()
            {
                return instance;
            }

            void Init(FsDevice *_device, FsRenderer *_renderer, VkDescriptorSetLayout _globalSetLayout);
            const PipelineKey GetOrCreatePipelineKey(const PipelineKey &_key);
            FsPipeline* GetPipeline(const PipelineKey &_key);
            VkPipelineLayout GetPipelineLayout(const PipelineKey &_key);
            void RecreatePipelinesFromShaderPath(const PipelineKey &_key);
            void RecreatePipelines();
            void Cleanup();  

        private:
            static FsShaderManager instance;
            FsDevice *device = nullptr;
            FsRenderer *renderer = nullptr;

            // TODO: Temporaire
            VkDescriptorSetLayout globalSetLayout = NULL;

            std::unordered_map<PipelineKey, PipelineData, PipelineKeyHasher> pipelineCache;

            VkPipelineLayout GetPipelineLayoutForKey(const PipelineKey& key);
    };
} 
