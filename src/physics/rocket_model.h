#pragma once

#include <vector>

#include "../math/MathTypes.h"
#include "../math/Quaternion.h"

#include "../physics/advanced_physics.hpp"

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

namespace ofsim_advanced_physics {
struct RocketModel {
  RigidBody body;
  std::vector<BodyElement> elements;
  dvec3 thrustDirection;
  f64 thrustMagnitude;
  f64 fuel;
  f64 fuel_mass;

  void update_fuel(f64 newFuel);
  void calculateLoads(void);

 private:
  void initialize(dvec3 initialPosition);
  void calcMassProperties(void);
  void calcFuelChamberMassProperties(void);
};
}  // namespace ofsim_advanced_physics