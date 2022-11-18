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
const double YAW         = 30.0;
const double PITCH       = -70.0;
const double SPEED       = 800;
const double SENSITIVITY = 0.1;
const double ZOOM        = 80.0;

class Camera
{
public:
    // camera Attributes
    glm::dvec3 position;
    glm::dvec3 rotationPosition;
    glm::dvec3 Front;
    glm::dvec3 Up;
    glm::dvec3 Right;
    glm::dvec3 WorldUp;

    // euler Angles
    double Yaw;
    double Pitch;
    float Roll;
    
    double rotationAngle = 0;

    // camera options
    double MovementSpeed;
    double movementSensitivity;
    double Zoom;

    bool automaticRotation = true;

    Camera(glm::dvec3 position = glm::dvec3(0.0, 0.0, 0.0), glm::dvec3 up = glm::dvec3(0.0, 1.0, 0.0),
        double yaw = YAW, double pitch = PITCH, float roll = 0.0f);
    Camera(double posX, double posY, double posZ, double upX, double upY, double upZ, double yaw, double pitch, double roll); 
    glm::dmat4 getViewMatrix();     
    void processCameraRotation(double xoffset, double yoffset, bool constrainPitch = true);
    
    void updatePosition(glm::dvec3 newPosition, glm::dvec3 rocketRotation);

    void processKeyboard(Camera_Movement direction, double deltaTime);
    void processMouseRotation(double xoffset, double yoffset, bool constrainPitch = true);

    void setAutomaticRotation(bool autorotation) { automaticRotation = autorotation; }

private:
    void updateCameraVectors();
};
#endif