#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Input {
    bool W{false}, A{false}, S{false}, D{false};
    bool RMB{false};
    bool MMB{false};
};

class Camera
{
public:
    void updateCamera();
    void resetCamera();

    glm::vec3 mPosition {0.0f, 0.5f, 3.0f};
    glm::vec3 mForward {0, 0, -1};
    glm::vec3 mRight {1, 0, 0};
    glm::vec3 mUp {0, 1, 0};

    glm::vec3 mMovementForward {0, 0, -1};

    float mPitch {0.f};
    float mYaw {0.f};

    glm::vec3 mCameraMovement {0,0,0};

    // Getters for Renderer compatibility
    glm::vec3 getPosition() const { return mPosition; }
    glm::vec3 getForward() const { return mForward; }
    glm::vec3 getUp() const { return mUp; }

    glm::mat4 getViewMatrix() const
    {
        return glm::lookAt(mPosition, mPosition + mForward, mUp);
    }
};

#endif // CAMERA_H
