# Changelog

Notable changes are listed by version, newest first. Changes awaiting release
are grouped under `Unreleased`. Release dates were not recorded for historical versions.

## Unreleased

## 0.27.7 alpha - 2026-11-15

### Changed

- Updated the splash screen version to 0.27.7 alpha and author years to 2021 - 2026.
- Improved logical grouping in the translator with blank lines and documented the convention.
- Updated the build instructions in `AGENTS.md` to use eight parallel jobs.

### Fixed

- Validated VM source translation and instruction decoding, with checked block transfers,
  bounded program images and controlled diagnostics for malformed programs.
- Corrected label resolution, translator reuse and self-jumps while preserving the bytecode
  generated for bundled assembly examples.
- Rejected invalid VM memory ranges and register indices with controlled errors, preserving
  valid end-of-memory accesses and replacing unsafe typed reads with byte copies.
- Initialized engine state, thrust telemetry and displacement consistently on first start
  and reset, with regression coverage for VM boundaries and physics initialization.
- Confined rocket and physics updates to the main loop, with a bounded flight-command queue,
  consistent telemetry snapshots and rejection of commands from stopped or previous runs.
- Preserved all command-history entries sharing the same simulation timestamp.

## 0.27.6 - 2026-11-14

### Added

- Concise project status report tracking completed work, open review findings and next steps.
- EditorConfig settings for consistent indentation, UTF-8 encoding and LF line endings.
- Project workflow, formatting and validation guidance in `AGENTS.md`.
- Regression tests for flight program execution and stopping.

### Changed

- Adopted a standalone LLVM-based clang-format configuration with four-space indentation,
  Allman braces and a 100-column limit.
- Formatted project C++ sources and tests with clang-format 19 and normalized line endings to LF.
- Organized this changelog into version sections with categorized unreleased changes.

### Fixed

- Unified asynchronous stopping for reset, control-mode changes and shutdown. Reset now waits
  for flight programs to finish while the interface remains responsive.
  Python programs must still cooperate with stop requests; forced termination is not yet supported.

### Removed

- Unused Google style clang-format submodule.

## 0.27.5

- Added this changelog, reorganized files and introduced the Google style clang-format submodule.

## 0.27.4

- Improved camera behavior and enabled physics in trajectory prediction mode.

## 0.27.3

- Added a GUI switch to enable or disable camera autorotation.

## 0.27.2

- Added a loading screen.

## 0.27.1

- Fixed the operation manual display.

## 0.27.0

- Added source code saving through the Save File dialog.

## 0.26.3

- Added filesystem navigation to the file dialog.

## 0.26.2

- Corrected the initial camera position in trajectory prediction mode.

## 0.26.1

- Added velocity and thrust vector indicators in manual control mode.

## 0.26.0

- Added manual rocket control using the keyboard.

## 0.25.1

- Corrected initial rocket orientation and planet rotation to match geographic coordinates.

## 0.25.0

- Added diagnostics mode.

## 0.24.1

- Completed virtual machine integration.

## 0.24.0

- Restored the previously removed virtual machine.

## 0.23.2

- Removed quaternions and simplified the initial rocket orientation code.

## 0.23.1

- Improved planet texturing, adjusted planet size and moved the coordinate origin to its center.

## 0.23.0

- Added Python integration and removed the virtual machine.

## 0.22.2

- Fixed simulation stopping to allow a full reset.

## 0.22.1

- Added line numbers to the code editor.
