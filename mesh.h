#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <string>
#include <vulkan/vulkan_core.h>
#include <array>
#include <map>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 textureCoordinate;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3 > getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, textureCoordinate);

        return attributeDescriptions;
    }
};

struct MaterialInfo
{
    std::string name;
    std::string diffuseTexture;
    glm::vec3 diffuseColor{0.8f, 0.8f, 0.8f};
    glm::vec3 ambientColor{0.2f, 0.2f, 0.2f};
    glm::vec3 specularColor{0.5f, 0.5f, 0.5f};
    float shininess{32.0f};
};

struct TempVertexData
{
    Vertex vertex;
    std::string materialName;
};

struct Texture
{
    Texture(std::string textureName) : mTextureName{ textureName }{};
    std::string mTextureName;

    int mTexWidth{0};
    int mTexHeight{0};
    int mTexChannels{0};
    uint32_t mMipLevels{1};

    VkImage mTextureImage{VK_NULL_HANDLE};
    VkDeviceMemory mTextureImageMemory{VK_NULL_HANDLE};
    VkImageView mTextureImageView{VK_NULL_HANDLE};
    VkDescriptorSet mTextureDescriptorSet{VK_NULL_HANDLE};
};

class Mesh
{
public:
    Mesh(std::string fileName);
    void makeObj();

    std::string getFileName() const { return mFileName; }

private:
    std::vector<Vertex> mVertices;
    std::vector<uint16_t> mIndices;
    std::string mFileName;

    std::map<std::string, MaterialInfo> mMaterials;

    VkBuffer mVertexBuffer{VK_NULL_HANDLE};
    VkBuffer mIndexBuffer{VK_NULL_HANDLE};
    VkDeviceMemory mVertexBufferMemory{VK_NULL_HANDLE};
    VkDeviceMemory mIndexBufferMemory{VK_NULL_HANDLE};

    void loadMaterialFile(const std::string& filename, std::map<std::string, MaterialInfo>& materials);

public:
    VkBuffer getVertexBuffer() const { return mVertexBuffer; }
    VkBuffer getIndexBuffer() const { return mIndexBuffer; }
    uint32_t getIndexCount() const { return static_cast<uint32_t>(mIndices.size()); }
    std::vector<Vertex>& getVertices() { return mVertices; }
    std::vector<uint16_t>& getIndices() { return mIndices; }

    void setVertexBuffer(VkBuffer buf) { mVertexBuffer = buf; }
    void setIndexBuffer(VkBuffer buf) { mIndexBuffer = buf; }
    void setVertexBufferMemory(VkDeviceMemory mem) { mVertexBufferMemory = mem; }
    void setIndexBufferMemory(VkDeviceMemory mem) { mIndexBufferMemory = mem; }

    VkDeviceMemory getVertexBufferMemory() const { return mVertexBufferMemory; }
    VkDeviceMemory getIndexBufferMemory() const { return mIndexBufferMemory; }

    const std::map<std::string, MaterialInfo>& getMaterials() const { return mMaterials; }
};

#endif // MESH_H
