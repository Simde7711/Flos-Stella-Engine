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
                glm::vec2 position;
                static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
                static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
            };

            LveModel(LveDevice &lveDevice, const std::vector<Vertex> &vertices);
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

            void CreateVertexBuffers(const std::vector<Vertex> &vertices);
    };
}