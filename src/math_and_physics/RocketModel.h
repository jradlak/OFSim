#pragma once;

#include "PhysicsMath.h"  

// glossary: 
//  ECS - external coordination system
//  LCS - local coordination system

struct RocketRigidBody
{
    f64         fTotalMass;         // total mass
    f64         fFuelMass;          // fuel mass
    Matrix      mInertia;           // moment of inertia
    Matrix      mInertiaInverse;    // inverted moment of inertia

    Vector      vPosition;          // rocket's position in the external coordination system (ECS)

    Vector      vVelocity;          // velocity in the ECS
    Vector      vVelocityBode;      // velocity in the LCS
    Vector      vAngularVelocity;   // angular velocity in the LCS 
    
    f64         fSpeed;             // absolute speed

    Quaternion  qOrientation;       // orientation in the LCS
    Vector      vEulerAngles;       // euler angles in the LCS

    Vector      vForces;            // sum of all forces acting on the rocket's body
    Vector      vMoments;           // resultant forces moment acting on the rocket's body
};