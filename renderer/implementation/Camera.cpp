#include "..\Camera.h"
#include "..\..\math_and_physics\Geometry.h"

// based on: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h

Camera::Camera(glm::dvec3 position, glm::dvec3 up,
    double yaw, double pitch, float roll)
    : Front(glm::dvec3(0.0f, 0.0f, -1.0)), MovementSpeed(SPEED), movementSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    Roll = roll;
    updateCameraVectors();
}

Camera::Camera(double posX, double posY, double posZ, double upX, double upY, double upZ, double yaw, double pitch, double roll)
    : Front(glm::dvec3(0.0, 0.0, -1.0)), MovementSpeed(SPEED), movementSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    position = glm::dvec3(posX, posY, posZ);
    WorldUp = glm::dvec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    Roll = roll;
    updateCameraVectors();
}

glm::dmat4 Camera::getViewMatrix()
{
    return glm::lookAt(rotationPosition + glm::dvec3(0.016, 0.0, 0.012), position, Up);
}

void Camera::processCameraRotation(double xoffset, double yoffset, bool constrainPitch)
{
    xoffset *= movementSensitivity;
    yoffset *= movementSensitivity;

    rotationAngle += xoffset;        

    glm::dvec3 pos = position + yoffset * 100;
    updatePosition(pos, glm::dvec3(0));
}

void Camera::updatePosition(glm::dvec3 newPosition, glm::dvec3 rocketRotation)
{    
    const float radius = 0.020;
    //rotationPosition = newPosition + glm::dvec3(0.0, 0.024, 0.0);
    double camX = cos(glm::radians(rotationAngle)) * radius;
    double camZ = sin(glm::radians(rotationAngle)) * radius;

    rotationPosition = glm::dvec3(-camZ / 2.0, camX, camZ) + newPosition;
    
    position = newPosition;
}

void Camera::updateCameraVectors()
{
    glm::dvec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));

    glm::mat4 roll_mat = glm::rotate(glm::mat4(1.0f), glm::radians(Roll), glm::vec3(Front));
    Up = glm::mat3(roll_mat) * Up;
}