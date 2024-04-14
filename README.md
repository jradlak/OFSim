# OFSim
Orbital Flight Simulator

**Things To Do**

ASAP:
1. Fix initial rocket orientation code. Rocket should be parallel to the normal vector at a point on the sphere. Additional correction shlouldn't be needed.
2. The rotation of the rocket does not completely coincide with the rotation of the thrust vector.

Less urgent:
1. Saving files in the source code editor. Currently, the editor only displays the loaded program text.
2. Correcting the position of the sun and moon. Currently, they are located polar to the planet, despite maintaining the correct distances.

**plans for the future**

**Change list**

0.24.1 - Fully working VM integration.

0.24.0 - Restore a previously removed virtual machine.

0.23.2 - Serious code reactorings: remove quaternions, and simplification of the inital rocket orientation code.

0.23.1 - Fixes in the planet texturying. Chaneg size of the planet. point (0, 0, 0) is now in the center of the planet and the origin of the simulation.

0.23.0 - Python integration. Remove Virtual Machine.

0.22.2 - Fixes simulation resetting process. Now full reset is possible when simulation is stopped.

0.22.1 - Code editor line numbers
