#pragma once

#include "PhysicsMath.h"  

#include <vector>

// Rocket structure:
//          /\
//         /  \      
//        /    \
//       | CONE |
//       |      |
//       |======|
//       |  F   |
//       |  U   |
//       |  E   |
//       |  L   |
//       |      |
//       |  V   |
//       |  A   |
//       |  R   |
//       |  I   |
//       |  A   |
//       |  B   |
//       |  L   |
//       |  E   |
//       |======|
//      /|      |\      
//     / |ENGINE| \
//    |  |\____/|  |
//    |  | \__/ |  |
//    | /  /__\  \ |
//    |/          \|

// Rocket consists of three elements:
//
// CONE and ENGINE sections have constant mass and center of mass
// FUEL section has variable mass (due to fuel burning) and center of mass

// glossary: 
//  ECS - external coordination system  
//  LCS - local coordination system

namespace ofsim_advanced_physics
{
    struct RigidBody
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

    struct BodyElement 
    {
        f64         fMass;
        Vector	    vDCoords;
        Vector	    vCGCoords;
        Vector	    vLocalInertia;
        f64 	    fIncidence;
        f64	        fDihedral;
        Vector	    vNormal;
        f64	        fArea;
    };

    struct RocketModel
    {
        RigidBody                   body;
        std::vector<BodyElement>    elements;
        Vector                      thrustDirection;
        f64                         thrustMagnitude;
        f64                         fuel;

        void updateFuel(f64 newFuel);
        void calcLoads(void);

        private:
            void initialize(Vector initialPosition);
            void calcMassProperties(void);
            void calcFuelChamberMassProperties(void);
    };
}