# Fum7X360

Experimental Fujitsu FM-7 emulator project targeting Xbox 360.

> **Status:** early emulator core. Check the latest GitHub Actions run for validation of the current commit. This is not yet a complete FM-7 emulator and does not produce a working Xbox 360 `default.xex`.

## Current implementation

- Portable C++17 core built with CMake.
- Initial Motorola 6809 instruction subset: immediate and extended loads/stores, basic arithmetic/logical operations, short and long branches, stack push/pull subset, `BSR`, `LBSR`, extended `JSR`, `RTS`, and basic cycle accounting.
- `MemoryBus` abstraction with 64 KiB address space, 8-bit reads/writes, big-endian 16-bit helpers, and wraparound at `0xFFFF`.
- CPU can use an injected `MemoryBus`; instruction fetch, stack access, and implemented extended load/store operations route through it.
- `FM7Machine` now exposes separate main-CPU and subsystem-CPU address spaces plus a shared-RAM window at different addresses in each space.
- Automated tests cover CPU operations, subroutine flow, bus byte/word access, and isolation/shared visibility in the machine memory scaffold.
- Host executable named `fum7x360`.

**Hardware-accuracy note:** this is still a development scaffold. Region constants are informed by the MAME FM-7 driver, but ROM banking, exact hardware decode, video hardware, I/O devices, arbitration/timing, and CPU integration into the machine bus are incomplete. The CPU class still uses a `MemoryBus` directly and has not yet been adapted to the machine's separate mapped read/write functions. Unsupported CPU opcodes stop execution, so the current subset cannot run FM-7 firmware or commercial software.

## Planned work

1. Connect each CPU through a memory interface that can route accesses through the machine's address decoder.
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
