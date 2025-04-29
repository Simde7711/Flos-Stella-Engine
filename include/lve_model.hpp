#pragma once

#include "lve_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm.hpp"

// std
#include <vector>

namespace lve
{
    class LveModel
    {
        public:
            struct Vertex
            {
                glm::vec3 position;
                glm::vec3 color;
                static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
                static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
            };

            struct Builder
            {
                std::vector<Vertex> vertices{};
                std::vector<uint32_t> indices{}; 
            };

            LveModel(LveDevice &lveDevice, const LveModel::Builder &builder);
            ~LveModel();

            LveModel(const LveModel &) = delete;
            LveModel &operator = (const LveModel &) = delete;

            void Bind(VkCommandBuffer commandBuffer);
            void Draw(VkCommandBuffer commandBuffer);

        private:
            LveDevice &lveDevice;
            VkBuffer vertexBuffer;
            VkDeviceMemory vertexBufferMemory;
            uint32_t vertexCount;

            bool hasIndexBuffer = false;
            VkBuffer indexBuffer;
            VkDeviceMemory indexBufferMemory;
            uint32_t indexCount;

            void CreateVertexBuffers(const std::vector<Vertex> &vertices);
            void CreateIndexBuffers(const std::vector<uint32_t> &indices);

    };
}