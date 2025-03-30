#include "rocket_model.h"

namespace ofsim_advanced_physics
{
    void RocketModel::update_fuel(f64 newFuel)
    {
        fuel = newFuel;
        calcFuelChamberMassProperties();
    }

    void RocketModel::calculateLoads(void)
    {
        dvec3 fb, mb;  // tmp forces and moments vectors

        // reset forces and moments:
        body.forces.x = 0;
        body.forces.x = 0;
        body.forces.x = 0;

        body.moments.x = 0;
        body.moments.x = 0;
        body.moments.x = 0;

        fb.x = 0;
        fb.y = 0; 
        fb.z = 0;

        mb.x = 0;
        mb.y = 0; 
        mb.z = 0;

        // TODO: calculate forces and moments:
        
    }

    void RocketModel::initialize(dvec3 initialPosition)
    {
        body.position = initialPosition;
    }

    void RocketModel::calcMassProperties(void)
    {
    }

    void RocketModel::calcFuelChamberMassProperties(void)
    {
    }
}