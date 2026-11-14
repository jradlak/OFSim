# Project guidance

OFSim is a C++20 orbital flight simulator with an OpenGL interface, embedded
Python flight programs and a custom assembly virtual machine.

## Build and validation

Run commands from the repository root. Configure a new build directory with:

```bash
cmake -S . -B build
```

On Linux, add `-DGLFW_BUILD_WAYLAND=OFF` to use X11 when Wayland development
tools are unavailable. Preserve existing CMake settings when reusing a build.

After changes that affect compilation or behavior, build and run the tests:

```bash
cmake --build build --target OFSim ofsim_tests -j 4
ctest --test-dir build --output-on-failure
```

Run the application from `build/` so it can locate its assets, shaders and docs.
Automated tests do not replace checking affected interactions in the GUI.
For documentation or editor configuration changes, use relevant checks instead
of rebuilding the application.

## Code and formatting

- Write identifiers, code comments, project documentation and commit messages
  in English. Follow the naming conventions of the surrounding code.
- Keep changes focused. Separate broad formatting or renaming from behavior
  changes.
- Follow `.clang-format` and `.editorconfig`. Use `clang-format-19` to format
  modified C++ files in `src/` and `tests/`.
- Preserve include order: OpenGL headers depend on GLAD preceding GLFW.
  Include sorting is intentionally disabled in `.clang-format`.
- Exclude `external_libraries/`, `libs/`, `build/` and assets from formatting.
  Change vendored dependencies only when the task requires it.
- Add regression tests for behavior changes where practical, especially around
  program execution, cancellation and object lifetimes.

Check formatting without modifying files:

```bash
rg --files -0 src tests -g '*.cpp' -g '*.h' -g '*.hpp' \
  | xargs -0 -r clang-format-19 --dry-run --Werror --style=file
```

## Changelog and completion

- Add concise `CHANGELOG.md` entries for meaningful changes. Describe major
  stages rather than individual implementation details.
- Keep one `Unreleased` section, grouped into `Added`, `Changed`, `Fixed` and
  `Removed` as applicable. Omit empty categories and list versions newest first.
  Preserve historical entries and do not invent release dates or version numbers.
- Update `doc/PROJECT_STATUS.md` when work changes a tracked status. Mark milestones
  complete only after their completion criteria have been verified.
- Summarize the result, checks performed and any remaining limitations.
- After changing code, configuration or documentation, end the response with
  a suggested short, single-line commit message in English. Match the repository
  style, using prefixes such as `fix:`, `feat:`, `refactor:`, `style:` or `chore:`.
