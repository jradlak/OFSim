#include "RocketModel.h"

namespace ofsim_advanced_physics
{
    void RocketModel::updateFuel(f64 newFuel)
    {
        fuel = newFuel;
        calcFuelChamberMassProperties();
    }

    void RocketModel::initialize(Vector initialPosition)
    {
        body.vPosition = initialPosition;
    }

    void RocketModel::calcMassProperties(void)
    {
    }

    void RocketModel::calcFuelChamberMassProperties(void)
    {
    }
}