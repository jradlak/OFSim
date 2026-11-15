# Project status and development roadmap

Updated: 2026-11-15.

The six items below retain the numbering from the initial project review.

## 1. Stopping, reset and program lifetime

**Status:** Partially complete.

Completed:

- Added a common asynchronous stopping lifecycle for Stop, reset, control-mode
  changes and shutdown in [Simulation](../src/simulation/Simulation.cpp).
- Deferred reset and VM memory cleanup until the worker has finished.
- Added atomic stop flags, preserved early stop requests, reset the simulation
  clock between runs and transferred execution errors to the main thread through
  [ProgramRunner](../src/simulation/ProgramRunner.h).

Remaining:

- [ ] Run Python flight programs in a separate process. An uncooperative script
  can currently keep stopping or shutdown pending indefinitely.
- [ ] Add graceful cancellation followed by forced termination after a deadline,
  including handling of worker crashes and startup failures.

## 2. Shared simulation state and thread synchronization

**Status:** Complete. Automated regression checks and manual GUI verification passed.

- [x] Keep rocket and physics updates in the main loop.
- [x] Replace direct worker mutations with a bounded, synchronized command queue.
- [x] Provide consistent telemetry snapshots instead of live object pointers.
- [x] Keep command history, errors and UI events on the main thread; publish simulation
  time with telemetry and retain the atomic Python stop flag.
- [x] Invalidate queued and late commands on Stop, mode changes and shutdown;
  publish the initial telemetry after reset and assign each run a fresh identifier.
- [x] Verify steering, pause, Stop/reset, mode changes and closing in the GUI with
  Python and VM programs after the communication change.

## 3. Physics and trajectory prediction

**Status:** Open. [PhysicsSolver](../src/physics/PhysicsSolver.cpp) uses constant
radial gravity and fuel consumption without a time factor. Altitude is calculated
outside the integration loop, and prediction uses different geometry.

Completed:

- Initialized the engine as off on construction and reset, with consistent thrust
  magnitude, direction-vector magnitude and rocket telemetry.
- Initialized the displacement baseline and cleared displacement when resetting or
  establishing orientation; added first-step and reset regression checks.

Remaining:

- [ ] Establish consistent units, planet radius and gravitational parameters.
- [ ] Implement inverse-square gravity and time-dependent fuel consumption.
- [ ] Recalculate altitude and check ground contact during each physics step.
- [ ] Use consistent geometry and gravity in simulation and prediction; define
  whether prediction represents coasting or powered flight.
- [ ] Clear all prediction buffers and bound the prediction horizon.
- [ ] Review integration accuracy and atmospheric drag.

## 4. VM memory access and Python API validation

**Status:** Partially complete. VM data access and register checks are hardened;
program decoding and Python API validation remain open.

Completed:

- Added overflow-safe range checks to byte, word and double-word memory accessors,
  preserving valid accesses ending at the memory boundary.
- Replaced unsafe typed reads in memory accessors and instruction operands with
  byte copies, preserving the existing VM data layout.
- Rejected invalid integer and floating-point register indices with descriptive
  exceptions instead of terminating the application; verified worker error delivery
  and subsequent execution in regression tests.

Remaining:

- [ ] Validate code size, instruction boundaries, opcodes and bulk memory copies
  during program loading and decoding.
- [ ] Validate register operands before execution, including translator conversions.
- [ ] Validate Python argument types, sequence lengths and numeric conversions.
- [ ] Report malformed programs and invalid API calls as controlled errors in the UI.
- [ ] Update example scripts that use the obsolete `orbital_thrust_change` API name.

## 5. Regression and physics tests

**Status:** Partially complete.

Completed:

- Added [regression tests](../tests/ProgramRunnerTests.cpp) for immediate and repeated
  stopping, worker cleanup, execution errors, VM memory preservation and Python reruns.
- Added [communication tests](../tests/EventProcessorTests.cpp) for deferred and ordered
  commands, queue overflow, stop/reset invalidation, concurrent access and Python/VM
  telemetry compatibility.
- Added memory and register boundary cases, overflowing addresses, worker error
  recovery and physics initialization/reset checks; the test suite passed with
  AddressSanitizer and UndefinedBehaviorSanitizer (leak detection excluded).
- Verified memory/register error messages, subsequent valid VM execution, coasting,
  powered flight and restoration of the initial engine state after reset in the GUI.
- Verified steering, pause, Stop/reset, mode changes and shutdown in the GUI with
  Python and VM programs.

Remaining:

- [ ] Add physical reference cases and checks for orbital energy, angular momentum,
  time-step sensitivity and prediction agreement.
- [ ] Extend invalid-input cases to malformed instruction streams and Python API
  arguments alongside the remaining item 4 work; use relevant sanitizers.

## 6. Reproducible builds and automated checks

**Status:** Open. [CMake](../CMakeLists.txt) fetches several dependencies from
`master` and uses deprecated GLM setup. Formatting and validation are still run manually.

- [ ] Pin dependencies to reviewed tags or commits and modernize their CMake setup.
- [ ] Verify a clean build and update build instructions to C++20, required system
  libraries and supported X11/Wayland options.
- [ ] Add reusable format and format-check commands using clang-format 19.
- [ ] Add CI for compilation, tests and formatting, with sanitizer checks where practical.

## Completed tooling and documentation work

- [x] Added a standalone [.clang-format](../.clang-format) and removed the Google style submodule.
- [x] Applied the shared style and LF line endings to project C++ sources and tests,
  preserving include order required by OpenGL.
- [x] Added [.editorconfig](../.editorconfig).
- [x] Recorded development conventions in [AGENTS.md](../AGENTS.md).
- [x] Structured [CHANGELOG.md](../CHANGELOG.md) by version and change type.

## Next task

Finish item 1 by moving Python into a separate process with bounded cancellation.
The command queue and telemetry snapshots now
provide the communication boundary for that work.

Then harden inputs in item 4 and improve physics in item 3, adding item 5 tests
with each change. Item 6 can proceed as separate maintenance work.
