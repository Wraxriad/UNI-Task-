#ifndef LIGHT_H
#define LIGHT_H

#include "gameobject.h"
#include <glm/glm.hpp>

class Light
{
public:
    Light();

    Transform mTransform;
    glm::vec3 lightColor{1.0f, 0.9f, 0.5f};

    float ambientStrength = 0.1f;
    float specularStrength = 0.5f;
    float specularExponent = 64.0f;
    float diffuseStrength = 1.0f;

    // Getters for Renderer compatibility
    glm::vec3 getPosition() const { return mTransform.position; }
    glm::vec3 getColor() const { return lightColor; }
    float getAmbient() const { return ambientStrength; }
    float getDiffuse() const { return diffuseStrength; }
    float getSpecular() const { return specularStrength; }
    float getShininess() const { return specularExponent; }

    // Setters
    void setPosition(glm::vec3 pos) { mTransform.position = pos; }
    void setColor(glm::vec3 col) { lightColor = col; }
};

#endif // LIGHT_H
