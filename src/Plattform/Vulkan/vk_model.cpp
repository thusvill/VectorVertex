#include "vk_model.hpp"
#include <Utils.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#define GLM_ENABLE_EXPERIMENTAL
#include <algorithm>
#include <glm/gtx/hash.hpp>
#include <unordered_map>
#include <tiny_obj_loader.h>
#include <cassert>
#include <cstring>
#include <Log.h>
#include <Application.hpp>

#include <iostream>
namespace std
{
    template <>
    struct hash<VectorVertex::VKModel::Vertex>
    {
        size_t operator()(VectorVertex::VKModel::Vertex const &vertex) const
        {
            size_t seed = 0;
            VectorVertex::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };
} // namespace std

namespace VectorVertex
{
    VKModel::VKModel(const Builder &builder)
    {
        CreateVertexBuffers(builder.vertices);
        CreateIndexBuffers(builder.indices);
    }

    VKModel::~VKModel()
    {
    }

    std::unique_ptr<VKModel> VKModel::createModelFromFile(const std::string &filepath)
    {
        VV_CORE_INFO(filepath);
        Builder builder{};
        builder.loadModel(filepath);
        return std::make_unique<VKModel>(builder);
    }
    void VKModel::Bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = {vertexBuffer->getBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        if (hasIndexBuffer)
        {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
        }
    }
    void VKModel::Draw(VkCommandBuffer commandBuffer)
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
    void VKModel::CreateVertexBuffers(const std::vector<Vertex> &vertices)
    {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "Vertex count must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

        uint32_t vertexSize = sizeof(vertices[0]);

        VKBuffer stagingBuffer{            vertexSize, vertexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void *)vertices.data());

        vertexBuffer = std::make_unique<VKBuffer>(
            vertexSize, vertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VKDevice::Get().copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
    }

    void VKModel::CreateIndexBuffers(const std::vector<uint32_t> &indices)
    {
        indexCount = static_cast<uint32_t>(indices.size());
        hasIndexBuffer = indexCount > 0;
        if (!hasIndexBuffer)
        {
            return;
        }
        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
        uint32_t indexSize = sizeof(indices[0]);
        VKBuffer stagingBuffer{
            indexSize, indexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};
        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void *)indices.data());

        indexBuffer = std::make_unique<VKBuffer>(
            indexSize, indexCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VKDevice::Get().copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
    }
    std::vector<VkVertexInputAttributeDescription> VKModel::Vertex::getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
        attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
        attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
        attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});

        return attributeDescriptions;
    }
    std::vector<VkVertexInputBindingDescription> VKModel::Vertex::getBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    void VKModel::Builder::loadModel(const std::string &filepath)
    {
        std::string lowerPath = filepath;
        std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), ::tolower);

        if (lowerPath.substr(lowerPath.length() - 4) == ".obj")
        {
            VV_CORE_INFO("Loading OBJ file: {0}", filepath);
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

            std::unordered_map<Vertex, uint32_t> uniqueVertex{};

            for (const auto &shape : shapes)
            {
                for (const auto &index : shape.mesh.indices)
                {
                    Vertex vertex{};

                    if (index.vertex_index >= 0)
                    {
                        vertex.position = {
                            attrib.vertices[3 * index.vertex_index + 0],
                            attrib.vertices[3 * index.vertex_index + 1],
                            attrib.vertices[3 * index.vertex_index + 2]};

                        vertex.color = {
                            attrib.colors[3 * index.vertex_index + 0],
                            attrib.colors[3 * index.vertex_index + 1],
                            attrib.colors[3 * index.vertex_index + 2]};
                    }

                    if (index.normal_index >= 0)
                    {
                        vertex.normal = {
                            attrib.normals[3 * index.normal_index + 0],
                            attrib.normals[3 * index.normal_index + 1],
                            attrib.normals[3 * index.normal_index + 2]};
                    }

                    if (index.texcoord_index >= 0)
                    {
                        vertex.uv = {
                            attrib.texcoords[2 * index.texcoord_index + 0],
                            attrib.texcoords[2 * index.texcoord_index + 1]};
                    }

                    if (uniqueVertex.count(vertex) == 0)
                    {
                        uniqueVertex[vertex] = static_cast<uint32_t>(vertices.size());
                        vertices.push_back(vertex);
                    }
                    indices.push_back(uniqueVertex[vertex]);
                }
            }
        }
        else if (lowerPath.substr(lowerPath.length() - 4) == ".fbx")
        {
           VV_CORE_WARN("Currently we'r not supprt FBX yet :/");
           return; 
           // VV_CORE_INFO("Loading FBX model: " + filepath);

           // loadFBX(filepath);
        }
        else{
            VV_CORE_ERROR("File Format {} not supported!", lowerPath.substr(lowerPath.length() - 4));
            return;
        }
    }

    void VKModel::Builder::loadFBX(const std::string &fbx_path)
{
    // Initialize FBX SDK Manager
    FbxManager *sdkManager = FbxManager::Create();
    if (!sdkManager)
    {
        throw std::runtime_error("Error: Unable to create FBX Manager!");
    }

    FbxScene *scene = FbxScene::Create(sdkManager, "My Scene");
    FbxImporter *importer = FbxImporter::Create(sdkManager, "");
    if (!importer->Initialize(fbx_path.c_str(), -1, sdkManager->GetIOSettings()))
    {
        throw std::runtime_error("Error: Unable to initialize FBX importer.");
    }

    if (!importer->Import(scene))
    {
        throw std::runtime_error("Error: Unable to import FBX file.");
    }
    importer->Destroy();

    // Initialize vertex and index data containers
    vertices.clear();
    indices.clear();
    std::unordered_map<Vertex, uint32_t> uniqueVertex{};

    // Process each node in the scene recursively
    FbxNode *rootNode = scene->GetRootNode();
    if (rootNode)
    {
        for (int i = 0; i < rootNode->GetChildCount(); ++i)
        {
            ProcessNode(rootNode->GetChild(i), uniqueVertex);
        }
    }

    // Clean up the SDK manager after processing
    sdkManager->Destroy();
}

void VKModel::Builder::ProcessNode(FbxNode *node, std::unordered_map<Vertex, uint32_t> &uniqueVertex)
{
    FbxMesh *mesh = node->GetMesh();
    if (!mesh)
    {
        return; // Skip this node if it does not contain a mesh
    }

    FbxVector4 *controlPoints = mesh->GetControlPoints();
    int polygonCount = mesh->GetPolygonCount();

    for (int i = 0; i < polygonCount; ++i)
    {
        int polygonSize = mesh->GetPolygonSize(i);
        for (int j = 0; j < polygonSize; ++j)
        {
            Vertex vertex{};

            // Get the index of the vertex in the control points array
            int controlPointIndex = mesh->GetPolygonVertex(i, j);
            FbxVector4 position = controlPoints[controlPointIndex];
            vertex.position = glm::vec3((float)position[0], (float)position[1], (float)position[2]);

            // Retrieve normals
            FbxVector4 normal;
            mesh->GetPolygonVertexNormal(i, j, normal);
            vertex.normal = glm::vec3((float)normal[0], (float)normal[1], (float)normal[2]);

            // Retrieve texture coordinates
            FbxVector2 uv;
            bool unmapped;
            mesh->GetPolygonVertexUV(i, j, mesh->GetElementUV()->GetName(), uv, unmapped);
            vertex.uv = glm::vec2((float)uv[0], (float)uv[1]);

            // Optional: Retrieve vertex colors (similar to normals/UVs)
            if (mesh->GetElementVertexColor())
            {
                FbxColor color = mesh->GetElementVertexColor()->GetDirectArray().GetAt(controlPointIndex);
                vertex.color = glm::vec3((float)color.mRed, (float)color.mGreen, (float)color.mBlue);
            }

            // Store unique vertices and create indices
            if (uniqueVertex.count(vertex) == 0)
            {
                uniqueVertex[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }
            indices.push_back(uniqueVertex[vertex]);
        }
    }
}

} // namespace VectorVertex
