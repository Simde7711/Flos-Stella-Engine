#pragma once

#include "fs.device.hpp"
#include "fs.buffer.hpp"

#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <string>
#include <memory>
#include <vector>

namespace fs
{
    class FsModel
    {
        public:
            struct Vertex
            {
                glm::vec3 position{};
                glm::vec3 color{};
                glm::vec3 normal{};
                glm::vec2 uv{};

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

            FsModel(FsDevice &device, const FsModel::Builder &builder);
            ~FsModel();

            FsModel(const FsModel &) = delete;
            FsModel &operator = (const FsModel &) = delete;

            static std::unique_ptr<FsModel> CreateModelFromFile(FsDevice &device, const std::string &filepath);

            void Bind(VkCommandBuffer commandBuffer);
            void Draw(VkCommandBuffer commandBuffer);

        private:
            FsDevice &device;

            std::unique_ptr<FsBuffer> vertexBuffer;
            uint32_t vertexCount;

            bool hasIndexBuffer = false;
            std::unique_ptr<FsBuffer> indexBuffer;
            uint32_t indexCount;

            void CreateVertexBuffers(const std::vector<Vertex> &vertices);
            void CreateIndexBuffers(const std::vector<uint32_t> &indices);

    };
}
