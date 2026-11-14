# Project status and development roadmap

Updated: 2026-11-14.

This report records the findings and work from the initial project review and
the subsequent development session. It separates completed work from open issues
so that tooling improvements do not obscure the simulator's technical priorities.
The numbering below preserves the original six review items.

## Current position

The common flight-program stopping lifecycle is implemented, but the broader
stability work is only partially complete. Python still runs in an application
thread, and simulation state is still shared without sufficient synchronization.

The last verified application and test builds succeeded on Linux using X11.
All 19 automated tests passed, including 10 new flight-program lifecycle tests.
These results do not establish full GUI reliability, thread safety or orbital accuracy.

| Original item | Area | Status | Priority |
| --- | --- | --- | --- |
| 1 | Stopping, reset and program lifetime | Partially complete | Highest |
| 2 | Shared simulation state and thread synchronization | Open | Highest |
| 3 | Physics and trajectory prediction | Open | High |
| 4 | VM memory access and Python API validation | Open | High |
| 5 | Meaningful regression and physics tests | Partially complete | Throughout the work |
| 6 | Reproducible dependencies and automated checks | Open | Medium |

## Completed work

### Application setup and verification

- [x] Configured and built the application with the available dependencies.
- [x] Used X11 with Wayland support disabled in the local build configuration.
- [x] Launched the application earlier in the session; the process exited successfully.
- [x] Rebuilt the application and tests after the code and formatting changes.

These are local verification results, not a guarantee that a fresh checkout builds
on every supported platform. No comprehensive GUI regression pass was completed.

### First stability stage: a common stopping lifecycle

- [x] Added [ProgramRunner](../src/simulation/ProgramRunner.h) to own the worker,
  collect completion and transfer execution exceptions to the main thread.
- [x] Added a `STOPPING` state shared by Stop, reset, control-mode changes and shutdown
  in [Simulation](../src/simulation/Simulation.cpp).
- [x] Kept servicing the window while waiting for the program to finish and disabled
  another start during that wait.
- [x] Deferred world-object reset until the worker has finished and been joined.
- [x] Changed VM stopping to request cancellation without clearing active memory.
- [x] Made stop flags atomic and preserved stop requests made before VM execution starts.
- [x] Reset the simulation clock and accumulated lag before another run.
- [x] Delivered Python errors after interpreter finalization rather than publishing
  them directly from its worker thread.
- [x] Added [lifecycle regression tests](../tests/ProgramRunnerTests.cpp) for immediate
  and repeated stopping, worker cleanup, exceptions, VM memory preservation and Python reruns.

Open limitation: an uncooperative Python program can keep the application in
`STOPPING` indefinitely. Normal shutdown keeps the window running while it waits;
the runner's destructor also has a blocking join as an unwinding fallback.
There is no forced-stop deadline or process isolation yet.

### Formatting and development workflow

- [x] Replaced the formatter-config link with a standalone [.clang-format](../.clang-format).
- [x] Adopted four-space indentation, Allman braces and a 100-column limit.
- [x] Removed the unused Google style formatter submodule.
- [x] Formatted all 84 project C++ files in `src/` and `tests/` with clang-format 19.
- [x] Normalized those C++ files to LF and verified that include order was preserved.
- [x] Disabled include sorting because GLAD must precede GLFW.
- [x] Added [.editorconfig](../.editorconfig), including exceptions for dependencies,
  build output, assets and Markdown whitespace.
- [x] Documented project conventions, verification commands and suggested commit messages
  in [AGENTS.md](../AGENTS.md).
- [x] Reorganized [CHANGELOG.md](../CHANGELOG.md) into one categorized `Unreleased`
  section and 19 historical version sections.
- [x] Documented how to run the formatter manually and how to enable EditorConfig in VS Code.

The fourth tooling step remains open: reusable formatting/check commands and CI.
Editor extensions and format-on-save settings have not been configured as part of this work.

## Remaining work and completion criteria

### 1. Finish program isolation and bounded shutdown

Python remains embedded in a thread. Cooperative stop flags cannot reliably stop
`while True: pass`, a long sleep or a blocking native extension.

- [ ] Run Python flight programs in a separate worker process.
- [ ] Exchange commands and telemetry through a defined communication interface.
- [ ] Request a graceful stop, then terminate the worker after a documented deadline.
- [ ] Associate messages with a run identifier and reject messages from previous runs.
- [ ] Handle worker crashes, startup failures and interpreter-finalization failures.

Completion criteria: infinite loops, long sleeps and worker failure cannot prevent
reset or application shutdown from completing within a bounded time. Check real
GUI Stop, reset, mode changes and window close, as well as automated cases.

### 2. Give simulation state a single owner

[EventProcessor](../src/infrastructure/EventProcessor.h) still exposes mutable rocket
and physics state directly. The main loop and flight-program workers access that
state, the command history and the simulation clock without a complete synchronization
strategy. The new atomic stop flags solve cancellation signaling only.

- [ ] Keep physics and rocket updates on one owning thread; the existing main loop
  can remain that owner initially.
- [ ] Replace direct worker mutations with a synchronized command queue.
- [ ] Give workers consistent telemetry snapshots instead of live object pointers.
- [ ] Define ownership for command history, errors, events and simulation time.
- [ ] Specify how queued commands are handled when stopping or resetting a run.
- [ ] Add concurrency regressions and use ThreadSanitizer where supported.

Completion criteria: workers cannot read or mutate live simulation objects directly;
reset cannot apply commands from an earlier run; relevant stress tests and sanitizer
checks support the ownership model.

### 3. Establish a consistent physics model

[PhysicsSolver](../src/physics/PhysicsSolver.cpp) currently uses constant-magnitude
radial gravity, consumes fuel per update without a time factor, and calculates altitude
before a loop that may perform multiple physics steps. Live and predicted altitude use
different body-size expressions. Prediction also retains old velocity samples and needs
defined behavior when elapsed time exceeds its prediction horizon.

- [ ] Document units, planet radius, coordinate origin and gravitational parameters.
- [ ] Use Newtonian gravity with acceleration proportional to inverse distance squared.
- [ ] Make fuel consumption depend on elapsed simulation time and enforce mass limits.
- [ ] Update altitude and ground-contact checks during each integration step.
- [ ] Use consistent geometry and gravity in live simulation and prediction.
- [ ] Define whether prediction describes coasting or powered flight.
- [ ] Clear all prediction buffers and bound the prediction horizon.
- [ ] Review integration accuracy and atmospheric drag after those foundations agree.

Completion criteria: analytical free-fall and circular-orbit cases behave as expected,
results converge with smaller time steps, and coasting prediction agrees with live
simulation within documented numerical tolerances.

### 4. Validate VM and Python inputs

[VM memory](../src/vmachine/Memory.cpp) permits a byte address equal to the buffer size,
uses unchecked raw copies and reads typed values through casts from byte arrays.
[Python callbacks](../src/python_integration/PythonMachine.cpp) do not fully validate
argument types, sequence length or numeric-conversion failures. Telemetry construction
also leaves temporary Python references unreleased.

- [ ] Validate complete memory ranges, including overflow-safe address/length checks.
- [ ] Replace unsafe typed reads with checked copies and define the VM byte layout.
- [ ] Validate loaded code size, instruction boundaries, opcodes and register operands.
- [ ] Return structured execution errors that the UI can display.
- [ ] Validate Python API arguments before using them and propagate Python exceptions correctly.
- [ ] Release temporary Python references and test repeated telemetry calls.

Completion criteria: malformed code, invalid addresses and invalid Python calls produce
controlled errors instead of corrupting memory or crashing the application. Exercise
boundary cases with regression tests and appropriate memory/undefined-behavior sanitizers.

### 5. Expand tests alongside implementation

Lifecycle coverage has improved, but the existing [physics test](../tests/PhysicsSolverTests.cpp)
does not establish motion accuracy. The automated suite does not exercise real GUI transitions.

- [ ] Add physical reference cases, including orbital energy and angular momentum checks.
- [ ] Test time-step sensitivity and prediction/live-simulation agreement.
- [ ] Test reset with active programs, stale commands and rapid mode changes.
- [ ] Add invalid-input and memory-boundary tests as item 4 is implemented.
- [ ] Define a repeatable GUI smoke checklist for manual and automatic control.

Completion criteria: tests reproduce the important failure modes and verify externally
meaningful behavior. Add them with the relevant fixes rather than postponing testing
until the end of the roadmap.

### 6. Make builds and checks reproducible

[CMake](../CMakeLists.txt) still fetches GLFW, GLAD and Assimp from `master`; the current
local build also relies on cached dependencies. GLM setup uses deprecated FetchContent
behavior. No CI workflow or reusable formatting targets/scripts have been added.

- [ ] Pin dependencies to reviewed tags or commits.
- [ ] Modernize dependency setup and verify a clean build directory.
- [ ] Document required system libraries, C++20 and the X11/Wayland build options.
- [ ] Add convenient format and format-check commands using clang-format 19.
- [ ] Add CI for compilation, tests and format checking, excluding third-party sources.
- [ ] Add sanitizer jobs once relevant failures can be investigated reliably.

Completion criteria: a clean checkout builds using documented dependencies and CI
checks the same formatter version and project commands used locally.

## Recommended next implementation slice

Start with original item 2: a command queue and telemetry snapshots owned by the
simulation thread. This addresses ongoing data races and creates the communication
boundary needed for a separate Python process.

Then finish item 1, harden VM/Python inputs in item 4, and improve physics in item 3.
Develop item 5 tests alongside each change. Dependency pinning and formatting CI
from item 6 can be handled as separate, focused maintenance work.

Also reconcile examples and documentation with the implementation: the README still
mentions C++17, and `src/orb_progs/orbital_prog1.py` calls the obsolete
`orbital_thrust_change` API name.

## Longer-term direction

The strongest proposed focus is an educational simulator where users program an
autopilot, inspect telemetry and learn to achieve a stable orbit around one planet.
This is a recommendation, not an agreed product commitment.

After the foundations above are reliable, consider reproducible flight scenarios,
result comparisons and more usable autopilot diagnostics. Decide whether Python or
the custom VM should be the primary control language before expanding both.

A full solar system, spacecraft construction and major graphics work should follow
a reliable one-planet flight model.

## Keeping this report useful

Update the date and affected statuses when a milestone is implemented and verified.
Mark an item complete only when its completion criteria are met, and retain any
remaining limitations. Keep detailed work planning here and concise release history
in `CHANGELOG.md`.
