#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include <vector>

// based on: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    ROLL_LEFT,
    ROLL_RIGHT
};

// Default camera values
const double YAW         = 90.0;
const double PITCH       = -90.0;
const double SPEED       = 0.005;
const double SENSITIVITY = 0.1;
const double ZOOM        = 45.0;

class Camera
{
public:
    // camera Attributes
    glm::dvec3 Position;
    glm::dvec3 Front;
    glm::dvec3 Up;
    glm::dvec3 Right;
    glm::dvec3 WorldUp;

    // euler Angles
    double Yaw;
    double Pitch;
    float Roll;
    
    // camera options
    double MovementSpeed;
    double MouseSensitivity;
    double Zoom;
    
    Camera(glm::dvec3 position = glm::dvec3(0.0, 0.0, 0.0), glm::dvec3 up = glm::dvec3(0.0, 1.0, 0.0),
        double yaw = YAW, double pitch = PITCH, float roll = 0.0f);
    Camera(double posX, double posY, double posZ, double upX, double upY, double upZ, double yaw, double pitch, double roll); 
    glm::dmat4 getViewMatrix();
    void processKeyboard(Camera_Movement direction, double deltaTime);    
    void processMouseMovement(double xoffset, double yoffset, bool constrainPitch = true);
    
private:
    void updateCameraVectors();
};
#endif