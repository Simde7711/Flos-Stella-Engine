#pragma once

#include "lve_device.hpp"
#include "lve_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm.hpp"

// std
#include <memory>
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
                glm::vec3 normal;
                glm::vec2 uv;

                static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
                static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
            
                bool operator==(const Vertex &other) const
                {
                    return position == other.position 
                    && color == other.color 
                    && normal == other.normal
                    && uv == other.uv;
                }
            };

            struct Builder
            {
                std::vector<Vertex> vertices{};
                std::vector<uint32_t> indices{}; 

                void LoadModel(const std::string &filepath);
            };

            LveModel(LveDevice &lveDevice, const LveModel::Builder &builder);
            ~LveModel();

            LveModel(const LveModel &) = delete;
            LveModel &operator = (const LveModel &) = delete;

            static std::unique_ptr<LveModel> CreateModelFromFile(LveDevice &device, const std::string &filepath);

            void Bind(VkCommandBuffer commandBuffer);
            void Draw(VkCommandBuffer commandBuffer);

        private:
            LveDevice &lveDevice;

            std::unique_ptr<LveBuffer> vertexBuffer;
            uint32_t vertexCount;

            bool hasIndexBuffer = false;
            std::unique_ptr<LveBuffer> indexBuffer;
            uint32_t indexCount;

            void CreateVertexBuffers(const std::vector<Vertex> &vertices);
            void CreateIndexBuffers(const std::vector<uint32_t> &indices);

    };
}