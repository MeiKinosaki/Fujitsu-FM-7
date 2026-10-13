# Fum7X360

Experimental Fujitsu FM-7 emulator project targeting Xbox 360.

> **Status:** early emulator core. The host build and existing smoke tests have passed in GitHub Actions previously; the renamed workflow must run again to verify the new target name. This is not yet a complete FM-7 emulator and does not produce a working Xbox 360 `default.xex`.

## Current implementation

- Portable C++17 core built with CMake.
- Initial Motorola 6809 instruction subset: loads/stores, basic arithmetic/logical operations, direct and long branches, stack push/pull subset, RTS and cycle accounting.
- Host smoke-test executable named `fum7x360`.
- GitHub Actions workflow that builds the host target, runs CTest and executes the smoke test.

Unsupported opcodes stop the current CPU core. This subset is intended for incremental development and is not sufficient to run FM-7 firmware or commercial software.

## Planned work

1. Expand 6809 instruction coverage and validate flags, cycles, and stack behavior with tests.
2. Separate CPU execution from memory access so the two processors can share a correct machine bus.
3. Implement the FM-7 memory map, shared memory and CPU handshake.
4. Implement video RAM/planes, palette and scanout.
5. Add sound, keyboard/joystick, cassette and disk devices.
6. Integrate the Xbox 360 frontend and choose a compatible SDK/toolchain.
7. Build and test a genuine Xbox 360 `default.xex` on supported hardware or a suitable test environment.

## Host build

Requires a C++17 compiler and CMake:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/fum7x360
```

This builds a host development executable, not an Xbox 360 executable.

## References

- [MAME FM-7 driver](https://github.com/mamedev/mame/blob/master/src/mame/fujitsu/fm7.cpp)
- [FM-7 technical notes](https://github.com/0cjs/sedoc/blob/main/8bit/fm7/fm-7.md)

Firmware/BIOS images and commercial software are not included. Use legally obtained files and keep them separate from the source tree.
