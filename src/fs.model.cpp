#include "fs.model.hpp"
#include "fs.utils.hpp"

// libs
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

// std
#include <cassert>
#include <cstring>
#include <iostream>
#include <unordered_map>

namespace std
{
    template<>
    struct hash<fs::FsModel::Vertex> 
    {
        size_t operator()( fs::FsModel::Vertex const &vertex) const 
        {
            size_t seed = 0;
            fs::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };
}

namespace fs
{
    FsModel::FsModel(FsDevice &device, const FsModel::Builder &builder) : device{device}
    {
        CreateVertexBuffers(builder.vertices);
        CreateIndexBuffers(builder.indices);

        std::cout << "[FsModel] Created from file, addr=" << this << "\n";
    }

    FsModel::~FsModel()
    {
       std::cout << "[FsModel] Destroyed, addr=" << this << "\n";
    }

    std::unique_ptr<FsModel> FsModel::CreateModelFromFile(FsDevice &device, const std::string &filepath)
    {
        Builder builder{};
        builder.LoadModel(filepath);
        // std::cout << "Vertex count: " << builder.vertices.size() << "\n";
        return std::make_unique<FsModel>(device, builder);
    }

    void FsModel::CreateVertexBuffers(const std::vector<Vertex> &vertices)
    {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "Vertex count mut be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
        uint32_t vertexSize = sizeof(vertices[0]);

        FsBuffer stagingBuffer
        {
            device,
            vertexSize,
            vertexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 

        };

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void *) vertices.data());

        vertexBuffer = std::make_unique<FsBuffer>
        (
            device,
            vertexSize,
            vertexCount, 
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        device.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
    }

    void FsModel::CreateIndexBuffers(const std::vector<uint32_t> &indices)
    {
        indexCount = static_cast<uint32_t>(indices.size());
        hasIndexBuffer = indexCount > 0;

        if (!hasIndexBuffer)
        {
            return;
        }

        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
        uint32_t indexSize = sizeof(indices[0]);

        FsBuffer stagingBuffer
        {
            device,
            indexSize,
            indexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        };
        
        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void *) indices.data());

        indexBuffer = std::make_unique<FsBuffer>
        (
            device,
            indexSize,
            indexCount, 
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        device.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
    }

    void FsModel::Bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = {vertexBuffer->getBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

        if (hasIndexBuffer)
        {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void FsModel::Draw(VkCommandBuffer commandBuffer)
    {
        if (hasIndexBuffer)
        {
            vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
        }
        else
        {
            vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
        }
    }

    std::vector<VkVertexInputBindingDescription> FsModel::Vertex::GetBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> FsModel::Vertex::GetAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        
        // Position
        attributeDescriptions.push_back
        ({
            0, 
            0,
            VK_FORMAT_R32G32B32_SFLOAT,
            offsetof(Vertex, position)
        });

        // Color
        attributeDescriptions.push_back
        ({
            1, 
            0,
            VK_FORMAT_R32G32B32_SFLOAT,
            offsetof(Vertex, color)
        });

        // Normal
        attributeDescriptions.push_back
        ({
            2, 
            0,
            VK_FORMAT_R32G32B32_SFLOAT,
            offsetof(Vertex, normal)
        });

        // uv
        attributeDescriptions.push_back
        ({
            3, 
            0,
            VK_FORMAT_R32G32_SFLOAT,
            offsetof(Vertex, uv)
        });

        return attributeDescriptions;
    }

    void FsModel::Builder::LoadModel(const std::string &filepath) 
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
        {
            throw std::runtime_error(warn + err);
        }

        vertices.clear();
        indices.clear();

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        for (const auto &shape : shapes)
        {
            for (const auto &index : shape.mesh.indices)
            {
                Vertex vertex{};

                if (index.vertex_index >= 0)
                {
                    vertex.position = 
                    {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2],
                    };

                    vertex.color = 
                    {
                        attrib.colors[3 * index.vertex_index + 0],
                        attrib.colors[3 * index.vertex_index + 1],
                        attrib.colors[3 * index.vertex_index + 2],
                    };
                }

                if (index.normal_index >= 0)
                {
                    vertex.normal = 
                    {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2],
                    };
                }

                if (index.texcoord_index >= 0)
                {
                    vertex.uv = 
                    {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1],
                    };
                }

                if (uniqueVertices.count(vertex) == 0)
                {
                   uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                   vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);
            }
        }
    }

}