#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

#include "../math_and_physics/MathTypes.h"

namespace ofsim_renderer
{
    enum Camera_Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        ROLL_LEFT,
        ROLL_RIGHT,
        NONE
    };

    // Initial camera values
    constexpr f64 YAW         { 30.0 };
    constexpr f64 PITCH       { -70.0 };
    constexpr f64 SPEED       { 400 };
    constexpr f64 SENSITIVITY { 0.1 };
    constexpr f64 ZOOM        { 80.0 };

    class Camera
    {
    public:
        // camera attributes
        dvec3 position,rotationPosition,
            front, up, right, worldUp;
            
        f64 yaw, pitch, roll;           
        f64 rotationAngle { 0 };

        // camera options:
        f64 MovementSpeed, movementSensitivity, Zoom;

        bool automaticRotation { true };

        Camera(dvec3 _position = dvec3(0.0, 0.0, 0.0), dvec3 _up = dvec3(0.0, 1.0, 0.0),
            f64 _yaw = YAW, f64 _pitch = PITCH, f64 _roll = 0.0);

        Camera(f64 posX, f64 posY, f64 posZ, f64 upX, f64 upY, f64 upZ, f64 _yaw, f64 _pitch, f64 _roll); 

        dmat4 getViewMatrix() { return automaticRotation ? 
            glm::lookAt(rotationPosition + glm::dvec3(0.016, 0.0, 0.012), position, up) 
            : glm::lookAt(position, position + front, up); }

        void setAutomaticRotation(bool autorotation) { automaticRotation = autorotation; }

        void processCameraRotation(f64 xoffset, f64 yoffset, bool constrainPitch = true);
        
        void updatePosition(dvec3 newPosition, dvec3 rocketRotation, f64 radius = 0.020);

        void processKeyboard(Camera_Movement direction, f64 deltaTime);
        void processMouseRotation(f64 xoffset, f64 yoffset, bool constrainPitch = true);

    private:
        void updateCameraVectors();
    };
}