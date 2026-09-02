#include "camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

void Camera::resetCamera()
{
    mCameraMovement = glm::vec3{0};
}

void Camera::updateCamera()
{
    glm::vec3 viewForward = glm::rotate(glm::vec3{0.f, 0.f, -1.f},
                                        glm::radians(mPitch),
                                        glm::vec3{1.0f, 0.0f, 0.0f});
    viewForward = glm::rotate(viewForward,
                              glm::radians(mYaw),
                              glm::vec3{0.0f, 1.0f, 0.0f});
    mForward = glm::normalize(viewForward);

    glm::vec3 moveForward = glm::rotate(glm::vec3{0.f, 0.f, -1.f},
                                        glm::radians(mYaw),
                                        glm::vec3{0.0f, 1.0f, 0.0f});
    mMovementForward = glm::normalize(moveForward);

    mRight = glm::rotate(glm::vec3{1, 0, 0},
                         glm::radians(mYaw),
                         glm::vec3{0.0f, 1.0f, 0.0f});
    mRight = glm::normalize(mRight);

    mUp = glm::vec3{0, 1, 0};

    mPosition += mMovementForward * mCameraMovement.z;

    mPosition += mRight * mCameraMovement.x;
}
