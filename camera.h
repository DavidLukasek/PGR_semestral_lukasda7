//----------------------------------------------------------------------------------------
/**
 * \file       camera.h
 * \author     David Lukasek
 * \date       2026/05/09
 * \brief      Camera class declaration.
 *
 *  Declares camera movement, orientation and view/projection matrix interfaces used by the renderer.
 *
*/
//----------------------------------------------------------------------------------------
#pragma once

#include "config.h"
#include "object.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * \brief      Camera class declaration.
 */
class Camera : public Object {
public:
    /**
     * \brief Creates a camera with default projection and movement settings.
     * \param initialPosition Initial camera position.
     * \param fieldOfViewDegrees Initial field of view in degrees.
     * \param nearPlane Near clipping plane.
     * \param farPlane Far clipping plane.
     * \param isSprinting Whether sprint mode starts enabled.
     */
    Camera(
        glm::vec3 initialPosition = glm::vec3(0.0f, 0.0f, 3.0f),
        float fieldOfViewDegrees = 90.0f,
        float nearPlane = NEAR_PLANE,
        float farPlane = FAR_PLANE,
        bool isSprinting = false
    );
    /// \brief Virtual destructor.
    ~Camera() override = default;

    /// \brief Returns the view matrix from current position and orientation.
    glm::mat4 getViewMatrix() const;
    /// \brief Returns the projection matrix for a given aspect ratio.
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

    /// \brief Moves camera forward along its front axis.
    void moveForward(float distance);
    /// \brief Moves camera backward along its front axis.
    void moveBackward(float distance);
    /// \brief Moves camera right along its right axis.
    void moveRight(float distance);
    /// \brief Moves camera left along its right axis.
    void moveLeft(float distance);
    /// \brief Moves camera up along world y axis.
    void moveUp(float distance);
    /// \brief Moves camera down along world y axis.
    void moveDown(float distance);

    /// \brief Changes yaw and pitch relatively.
    void rotate(float yawOffsetDegrees, float pitchOffsetDegrees);
    /// \brief Sets yaw and pitch absolutely.
    void setRotation(float yawDegrees, float pitchDegrees);

    /// \brief Changes field of view by a delta step.
    void changeFieldOfView(float delta);
    /// \brief Enables or disables sprint mode for movement.
    void setSprinting(bool value);

    /// \brief Returns normalized camera front direction.
    const glm::vec3& getFront() const;
    /// \brief Returns current camera up vector.
    const glm::vec3& getUp() const;
    /// \brief Returns current yaw in degrees.
    float getYaw() const;
    /// \brief Returns current pitch in degrees.
    float getPitch() const;
    /// \brief Returns current field of view.
    float getFieldOfView() const;

private:
    glm::vec3 front;               ///< Camera front direction.
    glm::vec3 up;                  ///< Camera local up vector.
    glm::vec3 right;               ///< Camera local right vector.
    glm::vec3 worldUp;             ///< World-space up reference vector.
    float yaw;                     ///< Horizontal camera rotation in degrees.
    float pitch;                   ///< Vertical camera rotation in degrees.
    float fieldOfViewDegrees;      ///< Camera field of view in degrees.
    float nearPlane;               ///< Projection near clipping plane.
    float farPlane;                ///< Projection far clipping plane.
    bool isSprinting;              ///< Whether sprint speed is used for movement.

    /// \brief Recalculates front/right/up vectors after rotation change.
    void updateVectors();
};
