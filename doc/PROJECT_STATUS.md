# Project status and development roadmap

Updated: 2026-11-15.

Status definitions:

- **Open:** No listed milestones have been completed and verified.
- **Partially complete:** Some work is complete and verified; listed work remains.
- **Complete:** All listed work is complete and the required checks have passed.

Completed work is marked `[x]`; remaining work is marked `[ ]`.

## 1. Stopping, reset and program lifetime

**Status:** Partially complete.

Completed:

- [x] Added a common asynchronous stopping lifecycle for Stop, reset, control-mode
  changes and shutdown in [Simulation](../src/simulation/Simulation.cpp).
- [x] Deferred reset and VM memory cleanup until the worker has finished.
- [x] Added atomic stop flags, preserved early stop requests, reset the simulation
  clock between runs and transferred execution errors to the main thread through
  [ProgramRunner](../src/simulation/ProgramRunner.h).

Remaining:

- [ ] Run Python flight programs in a separate process. An uncooperative script
  can currently keep stopping or shutdown pending indefinitely.
- [ ] Add graceful cancellation followed by forced termination after a deadline,
  including handling of worker crashes and startup failures.

## 2. Shared simulation state and thread synchronization

**Status:** Complete. Automated regression checks and manual GUI verification passed.

Completed:

- [x] Kept rocket and physics updates in the main loop.
- [x] Replaced direct worker mutations with a bounded, synchronized command queue.
- [x] Provided consistent telemetry snapshots instead of live object pointers.
- [x] Kept command history, errors and UI events on the main thread; published simulation
  time with telemetry and retained the atomic Python stop flag.
- [x] Invalidated queued and late commands on Stop, mode changes and shutdown;
  published the initial telemetry after reset and assigned each run a fresh identifier.
- [x] Verified steering, pause, Stop/reset, mode changes and closing in the GUI with
  Python and VM programs after the communication change.

## 3. Physics and trajectory prediction

**Status:** Partially complete. [PhysicsSolver](../src/physics/PhysicsSolver.cpp) uses constant
radial gravity and fuel consumption without a time factor. Altitude is calculated
outside the integration loop, and prediction uses different geometry.

Completed:

- [x] Initialized the engine as off on construction and reset, with consistent thrust
  magnitude, direction-vector magnitude and rocket telemetry.
- [x] Initialized the displacement baseline and cleared displacement when resetting or
  establishing orientation; added first-step and reset regression checks.
- [x] Preserved thrust direction independently of magnitude, including rotation at
  exactly zero thrust and restart without reset. Rendering uses the retained direction.
- [x] Bounded prediction to the remaining time before 8000 seconds, returning an empty
  prediction at or beyond the limit. Prediction and reset clear coordinate buffers;
  removed the unused prediction-speed buffer and initialized rendering geometry once
  so flight history can be displayed without predicted points.
- [x] Corrected [telemetry collection](../src/simulation/TelemetryCollector.cpp) to reset
  its sample baseline and calculate the signed rate of change of speed using actual
  millisecond timestamps, with a baseline sample at initialization and reset.
- [x] Kept all plot histories aligned within 2048 samples and bounded flight history to
  2048 points through progressive downsampling, retaining the first and latest points.
  The flight-path renderer includes the latest retained point.

Remaining:

- [ ] Establish consistent units, planet radius and gravitational parameters.
- [ ] Implement inverse-square gravity and time-dependent fuel consumption.
- [ ] Recalculate altitude and check ground contact during each physics step.
- [ ] Use consistent geometry and gravity in simulation and prediction; define
  whether prediction represents coasting or powered flight.
- [ ] Review integration accuracy and atmospheric drag.

## 4. VM memory access and Python API validation

**Status:** Complete. Automated regression checks and GUI error recovery verification passed.

Completed:

- [x] Added overflow-safe range checks to byte, word and double-word memory accessors,
  preserving valid accesses ending at the memory boundary.
- [x] Replaced unsafe typed reads in memory accessors and instruction operands with
  byte copies, preserving the existing VM data layout.
- [x] Rejected invalid integer and floating-point register indices with descriptive
  exceptions instead of terminating the application; verified worker error delivery
  and subsequent execution in regression tests.
- [x] Added synchronized, range-checked block transfers, made VM memory storage private
  and removed the unchecked `Memory::memcopy` helper.
- [x] Bounded both translation passes to the code buffer, counted `db` data when
  resolving labels and reset translator state between programs and after errors.
- [x] Validated source instructions, operand counts, labels, register conversions and
  numeric ranges; translation errors include the source line.
- [x] Bounded instruction fetching to the loaded image, rejecting empty programs,
  unknown opcodes, incomplete operands and invalid decoded register indices.
  Ordinary instructions advance explicitly, preserving direct, conditional and
  indirect self-jumps and stop requests.
- [x] Rejected zero divisors in integer division, modulo and floating-point division,
  including negative zero, and bounded bit shifts to the register width. Arithmetic
  errors identify the instruction and its address without changing the destination.
- [x] Validated Python API argument counts, rotation list/tuple lengths and numeric
  conversions, preserving conversion exceptions and rejecting non-finite values.
  Rotation conversion retains its elements even if user code mutates the input list.
- [x] Rejected non-finite flight commands from either language before enqueueing,
  validating whole rotations before inserting any of their component commands.
- [x] Preserved Python exception types in program diagnostics and verified controlled
  VM/Python errors followed by successful valid programs in tests and the GUI.
- [x] Updated the bundled Python example to use `orbital_thrust_magnitude_change`.

The loaded image includes instructions and `db` data; data access retains the full
VM address space. Validation checks complete instructions at the current address;
precomputed instruction-start validation and code/data separation are not implemented.

## 5. Regression and physics tests

**Status:** Partially complete.

Completed:

- [x] Added [regression tests](../tests/ProgramRunnerTests.cpp) for immediate and repeated
  stopping, worker cleanup, execution errors, VM memory preservation and Python reruns.
- [x] Added [communication tests](../tests/EventProcessorTests.cpp) for deferred and ordered
  commands, queue overflow, stop/reset invalidation, concurrent access and Python/VM
  telemetry compatibility.
- [x] Added memory and register boundary cases, overflowing addresses, worker error
  recovery and physics initialization/reset checks; the test suite passed with
  AddressSanitizer and UndefinedBehaviorSanitizer (leak detection excluded).
- [x] Verified memory/register error messages, subsequent valid VM execution, coasting,
  powered flight and restoration of the initial engine state after reset in the GUI.
- [x] Added block-copy concurrency and boundary checks, translator capacity/reuse cases,
  malformed instruction streams, loaded-image limits and cancellable self-jumps.
  Confirmed byte-for-byte translation compatibility for bundled assembly examples.
- [x] Verified source and runtime diagnostics, stopping self-jump loops, subsequent
  valid VM execution and repeated start/reset in the GUI.
- [x] Verified steering, pause, Stop/reset, mode changes and shutdown in the GUI with
  Python and VM programs.
- [x] Added regression checks for zero-thrust restart and rotation, prediction limits,
  repeated prediction and reset, telemetry sampling and reset, aligned plot windows
  and bounded flight history spanning the whole flight. Physics and telemetry tests
  passed with AddressSanitizer, UndefinedBehaviorSanitizer and leak detection enabled.
- [x] Verified zero-thrust rotation and restart, finite flight telemetry, signed speed-rate
  plots, pause and repeated prediction-mode changes in the GUI.
- [x] Added arithmetic boundaries, Python argument/conversion failures, mutation during
  rotation conversion, command rejection and valid reruns after errors. The full suite
  passed with AddressSanitizer and UndefinedBehaviorSanitizer (leak detection excluded).
- [x] Verified arithmetic and Python API error messages followed by successful VM and
  Python flight programs in the GUI.

Remaining:

- [ ] Add physical reference cases and checks for orbital energy, angular momentum,
  time-step sensitivity and prediction agreement.

## 6. Reproducible builds and automated checks

**Status:** Open. [CMake](../CMakeLists.txt) fetches several dependencies from
`master` and uses deprecated GLM setup. Formatting and validation are still run manually.

Remaining:

- [ ] Pin dependencies to reviewed tags or commits and modernize their CMake setup.
- [ ] Verify a clean build and update build instructions to C++20, required system
  libraries and supported X11/Wayland options.
- [ ] Add reusable format and format-check commands using clang-format 19.
- [ ] Add CI for compilation, tests and formatting, with sanitizer checks where practical.

## 7. Architecture and separation of responsibilities

**Status:** Open. Lower priority than bounded program cancellation and physics correctness;
the implementation stage has not been scheduled.

The qualitative architecture assessment is approximately 6/10 for the current project
scope. Separate physics, rendering, GUI and VM modules, together with the command queue,
telemetry snapshots and `ProgramRunner`, provide useful boundaries.

The greatest complexity is concentrated in [Simulation](../src/simulation/Simulation.cpp)
and its coordination with `ProgramRunner`, `EventProcessor` and the Python/VM interpreters.
It combines mode transitions, timing, program execution, world ownership, rendering and
GUI interactions. Correctness depends on operation ordering, especially during errors,
Stop, reset and shutdown. Creating the simulation also creates an OpenGL window/context,
which makes full lifecycle tests depend on the GUI.

[EventProcessor](../src/infrastructure/EventProcessor.h) is a global singleton combining
UI events, flight commands, telemetry, history and errors. This broad role makes
dependencies and state ownership harder to follow. Python's unbounded stopping behavior
is tracked separately in item 1 and should be addressed first.

Remaining:

- [ ] Extract a simulation core that can manage time, physics and telemetry without
  creating a window or an OpenGL context; connect the GUI and renderer through explicit
  inputs and outputs.
- [ ] Separate flight-program lifecycle coordination from `Simulation`, building on
  `ProgramRunner` and the process boundary introduced by item 1.
- [ ] Make mode transitions and their preconditions explicit, keeping Stop, reset,
  error handling and shutdown consistent across modes.
- [ ] Narrow the communication interfaces and pass dependencies explicitly, separating
  UI events/errors from worker command and telemetry exchange with clear state ownership.
- [ ] Add tests of the extracted core for start, pause, failure, Stop, reset and reruns,
  retaining GUI checks for the affected interactions.

Prefer small extractions alongside item 1 or 3 when they directly support those fixes.
Schedule broader restructuring after bounded cancellation and baseline physics correctness
have been verified. Use item 5 regression coverage and item 6 automated checks to protect
larger changes; splitting source files alone does not address the responsibility overlap.

## Completed tooling and documentation work

- [x] Added a standalone [.clang-format](../.clang-format) and removed the Google style submodule.
- [x] Applied the shared style and LF line endings to project C++ sources and tests,
  preserving include order required by OpenGL.
- [x] Added [.editorconfig](../.editorconfig).
- [x] Recorded development conventions in [AGENTS.md](../AGENTS.md).
- [x] Structured [CHANGELOG.md](../CHANGELOG.md) by version and change type.

## Next task

Finish item 1 by moving Python into a separate process with bounded
cancellation. The command queue and telemetry snapshots now provide the
communication boundary for that work.

Continue with the broader physics work in item 3, adding item 5 tests with each
change. Item 6 can proceed as separate maintenance work.

Keep item 7 as later architecture work, with its exact stage still to be decided.
Extract responsibilities incrementally when required by the higher-priority fixes.
