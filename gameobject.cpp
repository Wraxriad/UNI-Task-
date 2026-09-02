#include "gameobject.h"


GameObject::GameObject(Mesh* mesh, Material* material)
    : mMesh(mesh), mMaterial(material) {}

glm::mat4 GameObject::getModelMatrix() const
{
    return mTransfrom.getModelMatrix();
}
