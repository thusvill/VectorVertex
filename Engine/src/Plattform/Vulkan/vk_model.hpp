#pragma once
#include <Core/vvpch.hpp>
#include "vk_device.hpp"
#include "vk_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
// #include <fbxsdk.h>

#include <Render/RendererAPI.hpp>

namespace fbxsdk {
    class FbxManager;
    class FbxNode;
    class FbxScene;
    class FbxImporter;
}


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

            // Editor Only
            int enttID = -1;

            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();

            bool operator==(const Vertex &other) const
            {
                return position == other.position && color == other.color && normal == other.normal && uv == other.uv && enttID == other.enttID;
            }
        };
        struct Builder
        {
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
            fbxsdk::FbxManager *sdkManager;

            void
            loadModel(const std::string &filepath);
            void loadFBX(const std::string &fbx_path);
            void ProcessNode(fbxsdk::FbxNode *node, std::unordered_map<Vertex, uint32_t> &uniqueVertex);
        };
        VKModel(const Builder &builder);
        ~VKModel();

        VKModel(const VKModel &) = delete;
        VKModel &operator=(const VKModel &) = delete;

        static std::unique_ptr<VKModel>
        createModelFromFile(const std::string &filepath);

        void
        Bind(VkCommandBuffer commandBuffer);
        void
        BindWithEntityID(VkCommandBuffer commandBuffer, int entityID);
        void Draw(VkCommandBuffer commandBuffer);

        MeshData GetMeshData()
        {
            MeshData data;
            data.m_VertexBuffers = vertexBuffer;
            data.m_IndexBuffer = indexBuffer;
            data.m_IndexCount = indexCount;
            data.m_VertexCount = vertexCount;

            return data;
        }

    private:
        void CreateVertexBuffers(const std::vector<Vertex> &vertices);
        void CreateIndexBuffers(const std::vector<uint32_t> &indices);
        Ref<Buffer> vertexBuffer;
        uint32_t vertexCount;

        bool hasIndexBuffer = false;
        Ref<Buffer> indexBuffer;
        uint32_t indexCount;
        std::vector<Vertex> m_Vertices;
    };
} // namespace VectorVertex
