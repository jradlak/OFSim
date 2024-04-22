#pragma once

#include <vector>
#include <memory>

#include "Sun.h"
#include "Planet.h"
#include "Moon.h"
#include "Rocket.h"

namespace ofsim_world
{
    class SolarSystem
    {
    public:
        SolarSystem();

        void render(glm::dmat4 projection, glm::dmat4 view, glm::dvec3 _lightPos);

        void provideRocket(Rocket* _rocket) { rocket = _rocket; }
        void provideRocketInformationAndInit(double _angle, double _dangle, Rocket* _rocket);

        glm::dvec3 pointAboveEarthSurface(double theta, double phi, double distance) { return this->earth->pointAboveTheSurface(theta, phi, distance); }

        ~SolarSystem() {}

    private:
        std::unique_ptr<Planet> earth;
        std::unique_ptr<Moon> moon;
        std::unique_ptr<Sun> sun;

        Rocket* rocket;

        void initCloudsAndTreesForEarth(double _angle, double _dangle, glm::dvec3 _rotation) { earth->init(12, _angle, _dangle, _rotation); }
    };
}
