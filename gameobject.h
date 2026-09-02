#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Mesh;

struct Transform
{
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 rotation{0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};

    glm::mat4 getModelMatrix() const
    {
        glm::mat4 T = glm::translate(glm::mat4(1.0f), position);

        glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0)) *
                      glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0)) *
                      glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));

        glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
        return T * R * S;
    }
};

struct Material
{
    VkPipeline pipeline{VK_NULL_HANDLE};
    glm::vec4 color{0.5f, 0.5f, 0.5f, 1.0f};
    float shininess {64.0f};

    uint32_t textureIndex{0};

    virtual ~Material() = default;
};

class GameObject
{
public:
    GameObject(Mesh* mesh, Material* material);

    virtual ~GameObject() = default;

    Material* getMaterial() const { return mMaterial; }
    void setMaterial(Material* mat) {mMaterial = mat; }

    void setPosition(glm::vec3 pos) { mTransfrom.position = pos; }
    glm::vec3 getPosition() const { return mTransfrom.position; }

    void setRotation(glm::vec3 rot) { mTransfrom.rotation = rot; }
    glm::vec3 getRotation() const { return mTransfrom.rotation; }

    void setScale(glm::vec3 scale) {mTransfrom.scale = scale; }
    glm::vec3 getScale() const { return mTransfrom.scale; }

    Mesh* getMesh() const { return mMesh; }

    glm::mat4 getModelMatrix() const;

private:
    Mesh* mMesh{nullptr};
    Material* mMaterial{nullptr};

    Transform mTransfrom;
};

#endif // GAMEOBJECT_H
