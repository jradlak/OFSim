#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <thread>

#include "../world/SolarSystemConstants.h"
#include "TelemetryCollector.h"
#include "../renderer/Camera.h"
#include "../renderer/Window.h"
#include "../renderer/TrajectoryPrediction.h"
#include "../renderer/TrajectoryPrediction.h"
#include "../renderer/SkyBoxRenderer.h"
#include "../gui/Gui.h"
#include "../world/SolarSystem.h"
#include "../math_and_physics/PhysicsSolver.h"
#include "../math_and_physics/MathTypes.h"
#include "../python_integration/PythonMachine.h"
#include "../vmachine/VMachine.h"

namespace ofsim_simulation
{
    enum class SimulationMode
    {
        WAITING_FOR_BEGIN,
        STANDARD_SIMULATION,
        TRAJECTORY_PREDICTION,
        PRESENTATION_MODE,
        DIAGNOSTICS_MODE,
        FINISHED
    };

    struct Simulation
    {    
        Simulation();

        void init();

        void start();

        void stop();

    private:
        const i32 MS_PER_UPDATE{ 12 };

        const u32 SCR_WIDTH{ 1800 };
        const u32 SCR_HEIGHT{ 950 };

        //std::string SOURCE_CODE_FILE_NAME = "orbital_programs/ballisticProgram.oasm";
        std::string SOURCE_CODE_FILE_NAME{ "" };

        // render matrices:
        dmat4 projection;

        // world objects:
        std::unique_ptr<ofsim_world::SolarSystem> solarSystem;
        std::unique_ptr<ofsim_world::Rocket> rocket;

        // skybox:
        std::unique_ptr<SkyBoxRenderer> skyboxRenderer;

        // physics and trajectory prediction:
        std::unique_ptr<ofsim_math_and_physics::PhysicsSolver> physics;
        std::unique_ptr<TrajectoryPrediction> trajectoryPrediction;

        // camera, window and gui (user interaction objects):
        std::unique_ptr<ofsim_renderer::Camera> camera;
        std::unique_ptr<Window> mainWindow;
        std::unique_ptr<ofsim_gui::Gui> gui;

        // telemetry:
        std::unique_ptr<TelemetryCollector> telemetryCollector;

        // frame times:
        u64 lag;
        u64 previous;

        // simulation time variables:
        u64 startTime;
        u64 runningTime;
        u64 timePaused;

        // orbital orientation variables:
        f64 lastAltitude{ 0 };
        f64 apogeum{ 0 };
        f64 perygeum{ 0 };
        i32 lastAltitudeDirection{ 1 };
        i32 altitudeDirection{ 1 };

        f64 dTheta {ofsim_math_and_physics::theta};
        f64 dPhi {ofsim_math_and_physics::phi};

        // Python integration:
        std::unique_ptr<ofsim_python_integration::PythonMachine> pythonMachine { nullptr };
        std::unique_ptr<std::thread> pythonThread { nullptr };

        // VM integration:
        std::unique_ptr<ofsim_vm::VMachine> vmachine { nullptr };
        std::unique_ptr<std::thread> vmThread { nullptr };

        SimulationMode simulationMode{ SimulationMode::WAITING_FOR_BEGIN };

        std::string orbitalProgramSourceCode { "" };
        std::string orbitalProgramName { "" };

        dvec3 rocketInitialPosition(const f64 theta, const f64 phi) { return solarSystem
            ->pointAboveEarthSurface(theta, phi, SolarSystemConstants::earthSize); }

        void physicsRocketInitialOrientation();
        void initialSolarSystemInformation() { solarSystem->provideRocketInformationAndInit(ofsim_math_and_physics::theta, ofsim_math_and_physics::phi, rocket.get()); }
        void initialOrbitalInformation();

        void initWindowContext();
        void mainLoop();

        void terminatePythonMachine();
        void terminateVMachine();

        void renderHUD();

        void userInteractionLogic(dvec3 &toTheMoon, f64 &radius, f64 &step);

        void switchGLStateForWorldRendering(float r, float g, float b);
        void renderTelemetry(ofsim_gui::Gui* gui, ofsim_world::Rocket* rocket, double altitude, double apogeum, double perygeum, double atmosphereDragForceMagnitude);
        void calcApogeumAndPerygeum();

        void syncFramerate(u64 startTime, i32 ms_per_update);

        void createGui();

        void collectTelemetry();

        u64 currentTime();
    };
}
