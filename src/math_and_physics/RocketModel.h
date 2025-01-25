#pragma once

#include <vector>

#include "MathTypes.h"
#include "Quaternion.h"

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
        dmat4       mInertia;           // moment of inertia
        dmat4       mInertiaInverse;    // inverted moment of inertia

        dvec3       vPosition;          // rocket's position in the external coordination system (ECS)

        dvec3       vVelocity;          // velocity in the ECS
        dvec3       vVelocityBode;      // velocity in the LCS
        dvec3       vAngularVelocity;   // angular velocity in the LCS 
        
        f64         fSpeed;             // absolute speed

        Quaternion  qOrientation;       // orientation in the LCS
        dvec3       vEulerAngles;       // euler angles in the LCS

        dvec3       vForces;            // sum of all forces acting on the rocket's body
        dvec3       vMoments;           // resultant forces moment acting on the rocket's body
    };

    struct BodyElement 
    {
        f64         fMass;
        dvec3	    vDCoords;
        dvec3	    vCGCoords;
        dvec3	    vLocalInertia;
        f64 	    fIncidence;
        f64	        fDihedral;
        dvec3	    vNormal;
        f64	        fArea;
    };

    struct RocketModel
    {
        RigidBody                   body;
        std::vector<BodyElement>    elements;
        dvec3                       thrustDirection;
        f64                         thrustMagnitude;
        f64                         fuel;

        void updateFuel(f64 newFuel);
        void calculateLoads(void);

        private:
            void initialize(dvec3 initialPosition);
            void calcMassProperties(void);
            void calcFuelChamberMassProperties(void);
    };
}