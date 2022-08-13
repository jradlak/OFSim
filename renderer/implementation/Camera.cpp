#include "..\Camera.h"

// based on: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h

Camera::Camera(glm::dvec3 position, glm::dvec3 up,
    double yaw, double pitch, float roll)
    : Front(glm::dvec3(0.0f, 0.0f, -1.0)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    Roll = roll;
    updateCameraVectors();
}

Camera::Camera(double posX, double posY, double posZ, double upX, double upY, double upZ, double yaw, double pitch, double roll)
    : Front(glm::dvec3(0.0, 0.0, -1.0)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = glm::dvec3(posX, posY, posZ);
    WorldUp = glm::dvec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    Roll = roll;
    updateCameraVectors();
}

glm::dmat4 Camera::getViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);;
}

void Camera::processKeyboard(Camera_Movement direction, double deltaTime)
{
    double velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
    {
        Position += Front * velocity;
    }

    if (direction == BACKWARD)
    {
        Position -= Front * velocity;
    }

    if (direction == LEFT)
    {
        Position -= Right * velocity;
    }

    if (direction == RIGHT)
    {
        Position += Right * velocity;
    }

    if (direction == ROLL_LEFT)
    {
        Roll -= 1.0f;
        if (Roll < -89.0)
        {
            Roll = -89.0;
        }
    }

    if (direction == ROLL_RIGHT)
    {
        Roll += 1.0f;
        if (Roll > 89.0)
        {
            Roll = 89.0;
        }
    }

    updateCameraVectors();
}

void Camera::processCameraRotation(double xoffset, double yoffset, bool constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch)
    {
        if (Pitch > 89.0)
        {
            Pitch = 89.0;
        }

        if (Pitch < -89.0)
        {
            Pitch = -89.0;
        }
    }

    updateCameraVectors();
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