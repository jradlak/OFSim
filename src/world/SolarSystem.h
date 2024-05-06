#pragma once

#include <vector>
#include <memory>
#include "Rocket.h"
#include "CelestialBody.h"
#include "CloudsAndTrees.h"
#include "Launchpad.h"
#include "../math_and_physics/MathTypes.h"

namespace ofsim_world
{   
    struct Planet
    {
        Planet(f64 _size, const glm::dvec3& _position);

        void render(glm::dmat4 projection, glm::dmat4 view, const glm::dvec3 lightPos);

        void init(i32 number, f64 _angle, f64 _dangle, dvec3 _rotation);

        dvec3 pointAboveTheSurface(f64 theta, f64 phi, f64 distance) { return celestialBody->pointAboveTheSurface(theta, phi, distance); }

    private:
        std::unique_ptr<CelestialBody> celestialBody = nullptr;
        std::unique_ptr<CloudsAndTrees> cloudsAndTrees = nullptr;
        std::unique_ptr<Launchpad> launchpad = nullptr;
    };

    struct Moon
    {
        Moon(double _size, const glm::dvec3 _position);
        void render(glm::dmat4 projection, glm::dmat4 view, const glm::dvec3 lightPos) { celestialBody->render(projection, view, lightPos); }

    private:
        std::unique_ptr<CelestialBody> celestialBody;
    };

    struct Sun
    {
        Sun(double _size, const glm::dvec3 _position);

        void render(glm::dmat4 projection, glm::dmat4 view, glm::dvec3 lightPos) { celestialBody->render(projection, view, lightPos); }

    private:
        std::unique_ptr<CelestialBody> celestialBody;
    };

    struct SolarSystem
    {
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
