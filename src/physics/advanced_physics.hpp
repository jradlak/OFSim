#pragma once

#include "../math/MathTypes.h"  

namespace ofsim_advanced_physics {

    // TODO!
    class AdvancedPhysics
    {
        public:
            AdvancedPhysics() = default;
            ~AdvancedPhysics() = default;

            void updateFuel(f64 newFuel);
            void calculateLoads(void);
            void initialize(dvec3 initialPosition);
            void calcMassProperties(void);
            void calcFuelChamberMassProperties(void);
    };

}