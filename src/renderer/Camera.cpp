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
    automaticRotation = true;

    xoffset *= movementSensitivity;
    yoffset *= movementSensitivity;

    rotationAngle += xoffset;            
}

void Camera::processKeyboard(Camera_Movement direction, f64 deltaTime)
{
    if (automaticRotation) return;

    f64 velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
    {
        position += front * velocity;
    }

    if (direction == BACKWARD)
    {
        position -= front * velocity;
    }

    if (direction == LEFT)
    {
        position -= right * velocity;
    }

    if (direction == RIGHT)
    {
        position += right * velocity;
    }

    updateCameraVectors();
}

void Camera::processMouseRotation(f64 xoffset, f64 yoffset, bool constrainPitch)
{
    if (automaticRotation || manualRotation) return;

    xoffset *= movementSensitivity;
    yoffset *= movementSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (constrainPitch)
    {
        pitch = pitch > 89.0 ? 89.0 : pitch;
        pitch = pitch < -89.0 ? -89.0 : pitch;        
    }
    
    updateCameraVectors();
}

void Camera::updateAutomaticRotationPosition(dvec3 newPosition, f64 radius)
{    
    f64 camX = sin(glm::radians(rotationAngle)) * radius;
    f64 camZ = cos(glm::radians(rotationAngle)) * radius;

    rotationPosition = dvec3(camX, 0, camZ) + newPosition; 
    position = newPosition;    
}

void Camera::updateManualRotationPosition(f64 xOffset, f64 yOffset, f64 radius)
{
    if (automaticRotation || !manualRotation) return;

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
        camZ = cos(glm::radians(rotationAngle)) * radius;
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