#pragma once

#include "vv_device.hpp"
#include "vv_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <fbx/include/fbxsdk.h>
#include <vector>
namespace VectorVertex
{

    class VVModel
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
        VVModel(const Builder &builder);
        ~VVModel();

        VVModel(const VVModel &) = delete;
        VVModel &operator=(const VVModel &) = delete;

        static std::unique_ptr<VVModel>
        createModelFromFile(const std::string &filepath);

        void
        Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);

    private:
        void CreateVertexBuffers(const std::vector<Vertex> &vertices);
        void CreateIndexBuffers(const std::vector<uint32_t> &indices);
        std::unique_ptr<VVBuffer> vertexBuffer;
        uint32_t vertexCount;

        bool hasIndexBuffer = false;
        std::unique_ptr<VVBuffer> indexBuffer;
        uint32_t indexCount;
    };
} // namespace VectorVertex
