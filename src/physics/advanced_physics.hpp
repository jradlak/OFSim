#pragma once

#include "../math/MathTypes.h"
#include "../math/Quaternion.h"

namespace ofsim_advanced_physics {

// glossary:
//  ECS - external coordination system
//  LCS - local coordination system

struct RigidBody {
  f64 total_mass;         // total mass
  dmat4 inertia;          // moment of inertia
  dmat4 inertia_inverse;  // inverted moment of inertia

  dvec3 position;  // bodys's position in the external coordination system (ECS)

  dvec3 velocity;          // velocity in the ECS
  dvec3 velocity_node;     // velocity in the LCS
  dvec3 angular_velocity;  // angular velocity in the LCS

  f64 speed;  // absolute speed

  Quaternion orientation;  // orientation in the LCS
  dvec3 euler_angles;      // euler angles in the LCS

  dvec3 forces;   // sum of all forces acting on the rigid body
  dvec3 moments;  // resultant forces moment acting on the rigid body
};

struct BodyElement {
  f64 mass;
  dvec3 d_coords;
  dvec3 c_g_coords;
  dvec3 local_inertia;
  f64 incidence;
  f64 dihedral;
  dvec3 normal;
  f64 area;
};

class AdvancedPhysics {
 public:
  AdvancedPhysics() = default;
  ~AdvancedPhysics() = default;

  
};

}  // namespace ofsim_advanced_physics