# Fum7X360

Experimental Fujitsu FM-7 emulator project targeting Xbox 360.

> **Status:** early emulator core. Check the latest GitHub Actions run for validation of the current commit. This is not yet a complete FM-7 emulator and does not produce a working Xbox 360 `default.xex`.

## Current implementation

- Portable C++17 core built with CMake.
- Initial Motorola 6809 instruction subset: immediate and extended loads/stores, basic arithmetic/logical operations, short and long branches, stack push/pull subset, `BSR`, `LBSR`, extended `JSR`, `RTS`, and basic cycle accounting.
- `MemoryInterface` abstraction for CPU byte/word memory operations, with the flat `MemoryBus` implementing the interface.
- `M6809` now accepts any `MemoryInterface`, so instruction fetch, stack access and implemented data operations can be routed through a machine-level decoder.
- `FM7Machine` implements the main CPU memory interface and maps accesses to the main address space, including the shared-RAM window.
- A CPU integration test runs code through `FM7Machine` and verifies that a write to main shared RAM is visible through the subsystem shared-RAM address.
- Separate main and subsystem backing buses and an initial shared-RAM window.
- Host executable named `fum7x360`.

**Hardware-accuracy note:** this remains a development scaffold. Region constants need validation against hardware documentation and the reference driver. ROM banking, peripheral decoding, video hardware, I/O devices, arbitration/timing, and subsystem CPU interface integration are incomplete. Unsupported CPU opcodes stop execution, so this cannot yet run FM-7 firmware or commercial software.

## Planned work

1. Add the subsystem CPU memory adapter and route it through the subsystem address decoder.
2. Validate the map against reference implementations and documentation; implement ROM/RAM banking and peripheral decode.
3. Add shared-memory access timing and processor coordination.
4. Expand and validate 6809 instruction coverage, flags, cycle timing, and interrupt handling.
5. Implement video RAM/planes, palette and scanout.
6. Add sound, keyboard/joystick, cassette and disk devices.
7. Integrate the Xbox 360 frontend and select a compatible SDK/toolchain.
8. Build and test a genuine Xbox 360 `default.xex` on supported hardware or a suitable test environment.

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
