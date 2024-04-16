# OFSim
Orbital Flight Simulator

**=====Introduction:=====**

The idea of the program is to simulate ballistic flights (including orbital) of a rocket in the gravitational field of a planet with parameters similar to Earth. 
The rocket is controlled by a specially designed virtual computer (virtual machine). 
Rocket control involves writing programs in the assembly language of the virtual machine. 
The assembly program has access to the computer's memory, where telemetry data of the rocket's flight is stored.Based on this data,
the program can issue commands to the rocket, such as changing the direction and length of the thrust vector, controlling a set of rotating gyroscopes in orbit, etc.

In addition to the built-in assembly language, it is possible to write flight programs in Python.

The program includes a special mode that displays a graph of the rocket's predicted trajectory. 
The simulation is performed in real time and on a real scale.

**=====Compilation guide:=====**

Prerequirements:

Due to the fact that the program uses Python as one of the rocket flight control languages, 
it is required to install Python development libraries. On Windows, simply select the option to install development libraries in the Python installer. Linux requires instalation python-dev package:
```
(Debian/Ubuntu):

sudo apt install python3-dev
```
Additionally, Linux requires the installation of OpenGL and the other libraries needed to compile the project's dependent libraries. Under Windows everything should be already present.
```
(Debian/Ubuntu):

sudo apt install libgl1-mesa-dev libxrandr-dev libxi-dev libxinerama-dev libxcursor-dev pkg-config libxkbcommon-x11-dev
```

Set of libraries may differ on your system. Pay attention to CMake messages during compilation process.

Compilation:

Project requires CMake and C++ (g++ or MSVC++) compiler supporting at least C++17.
```
(in the project's home directory):

cmake -S . -B build
cmake --build build

cd build/
./OFSim
```
If you use Visual Studio 2022 on Windows, you can import the project into the IDE - the configuration should be automatic.

**=====Screenshots:=====**

![Trajectory prediction mode](https://github.com/jradlak/OFSim/blob/master/doc/Images/trajectoryPrediction.jpg)
![Excentric orbit](https://github.com/jradlak/OFSim/blob/master/doc/Images/excentricOrbit.png)


**=====Quick start guide:=====**

1. To start the simulation, you must load the flight control program. In menu (Program->Open...) Manual control is not supported.
2. Sample flight programs are available in the folder: src/orb_progs. Python and oasm (virtual machine) source codes are supported. The simulator will automatically recognize the program type and launch the appropriate interpreter.
3. The program allows you to pause and reset the simulation. Time acceleration is also available. 
4. Additional instructions on how to use the program can be found in the doc directory.

**=====Things To Do:=====**

ASAP:
1. Fix initial rocket orientation code. Rocket should be parallel to the normal vector at a point on the sphere. Additional correction shlouldn't be needed.
2. The rotation of the rocket does not completely coincide with the rotation of the thrust vector.

Less urgent:
1. Saving files in the source code editor. Currently, the editor only displays the loaded program text.
2. Correcting the position of the sun and moon. Currently, they are located polar to the planet, despite maintaining the correct distances.

**=====Plans for the future:=====**

1. Simulation of the entire solar system in real time and at true scale. Currently, the simulator only includes the Earth, Moon and Sun.
2. Dynamics of the Solar system in real time.
3. Better rocket flight control programs, including more types of orbits, gravity assists, etc. 
4. Better physics simulation. Right now the rocket is physically treated as a ball of variable mass. The forces acting on the rocket do not take into account its shape and mass distribution. 
5. Ability to build your own spaceships.
6. Better graphics. Shadows, tesselation of the terrain. 

**=====Changelog:=====**

0.24.1 - Fully working VM integration.

0.24.0 - Restore a previously removed virtual machine.

0.23.2 - Serious code reactorings: remove quaternions, and simplification of the inital rocket orientation code.

0.23.1 - Fixes in the planet texturying. Change size of the planet. Point (0, 0, 0) is now in the center of the planet and the origin of the simulation.

0.23.0 - Python integration. Remove Virtual Machine.

0.22.2 - Fixes simulation resetting process. Now full reset is possible when simulation is stopped.

0.22.1 - Code editor line numbers.

