#include "Camera.h"

using namespace ofsim_renderer;

Camera::Camera(dvec3 _position, dvec3 _up,
    f64 _yaw, f64 _pitch, f64 _roll)
    : front(dvec3(0.0f, 0.0f, -1.0)), MovementSpeed(SPEED), movementSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    position = _position;
    worldUp = _up;
    yaw = _yaw;
    pitch = _pitch;
    roll = _roll;
    updateCameraVectors();
}

Camera::Camera(f64 posX, f64 posY, f64 posZ, f64 upX, f64 upY, f64 upZ, f64 _yaw, f64 _pitch, f64 _roll)
    : front(dvec3(0.0, 0.0, -1.0)), MovementSpeed(SPEED), movementSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    position = dvec3(posX, posY, posZ);
    worldUp = dvec3(upX, upY, upZ);
    yaw = _yaw;
    pitch = _pitch;
    roll = _roll;
    updateCameraVectors();
}

void Camera::processCameraRotation(f64 xoffset, f64 yoffset, bool constrainPitch)
{
    if (manualRotation) return;
    
    xoffset *= movementSensitivity;
    yoffset *= movementSensitivity;

    rotationAngle += xoffset;
}

void Camera::updateAutomaticRotationPosition(dvec3 newPosition, f64 radius)
{
    if (manualRotation) return;

    f64 camX = sin(glm::radians(rotationAngle)) * radius;
    f64 camZ = cos(glm::radians(rotationAngle)) * radius;

    rotationPosition = dvec3(camX, 0, camZ) + newPosition; 
    position = newPosition;    
}

void Camera::updateManualRotationPosition(f64 xOffset, f64 yOffset, f64 radius)
{
    if (!manualRotation) return;

    f64 camX = 0, camY = 0, camZ = 0;

    if (xOffset != 0)
    {
        rotationAngle += xOffset * movementSensitivity;
        camX = sin(glm::radians(rotationAngle)) * radius;
        camZ = cos(glm::radians(rotationAngle)) * radius;
    }

    if (yOffset != 0)
    {
        rotationAngle += yOffset * movementSensitivity;
        camY = sin(glm::radians(rotationAngle)) * radius;
        camX = cos(glm::radians(rotationAngle)) * radius;
    }

    rotationPosition = dvec3(camX, camY, camZ) + position;
}

void Camera::updateCameraVectors()
{
    dvec3 _front;
    _front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    _front.y = sin(glm::radians(pitch));
    _front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = normalize(_front);

    right = normalize(cross(front, worldUp));
    up = normalize(cross(right, front));
}
