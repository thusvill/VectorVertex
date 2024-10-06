#pragma once
#include <vvpch.hpp>
#include "vk_device.hpp"
#include "vk_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <fbxsdk.h>
namespace VectorVertex
{

    class VKModel
    {
    public:
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 color;
            glm::vec3 normal{};
            glm::vec2 uv{};
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();

            bool operator==(const Vertex &other) const
            {
                return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
            }
        };
        struct Builder
        {
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
            FbxManager *sdkManager;

            void
            loadModel(const std::string &filepath);
            void loadFBX(const std::string &fbx_path);
            void ProcessNode(FbxNode *node, std::unordered_map<Vertex, uint32_t> &uniqueVertex);
        };
        VKModel(const Builder &builder);
        ~VKModel();

        VKModel(const VKModel &) = delete;
        VKModel &operator=(const VKModel &) = delete;

        static std::unique_ptr<VKModel>
        createModelFromFile(const std::string &filepath);

        void
        Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);

    private:
        void CreateVertexBuffers(const std::vector<Vertex> &vertices);
        void CreateIndexBuffers(const std::vector<uint32_t> &indices);
        std::unique_ptr<VKBuffer> vertexBuffer;
        uint32_t vertexCount;

        bool hasIndexBuffer = false;
        std::unique_ptr<VKBuffer> indexBuffer;
        uint32_t indexCount;
    };
} // namespace VectorVertex
