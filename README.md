# Fum7X360

Experimental Fujitsu FM-7 emulator project targeting Xbox 360.

> **Status:** early emulator core. Check the latest GitHub Actions run for validation of the current commit. This is not yet a complete FM-7 emulator and does not produce a working Xbox 360 `default.xex`.

## Current implementation

- Portable C++17 core built with CMake.
- Initial Motorola 6809 instruction subset: immediate and extended loads/stores, basic arithmetic/logical operations, short and long branches, stack push/pull subset, `BSR`, `LBSR`, extended `JSR`, `RTS`, and basic cycle accounting.
- `MemoryBus` abstraction with 64 KiB address space, 8-bit reads/writes, big-endian 16-bit helpers, and wraparound at `0xFFFF`.
- CPU can use an injected shared `MemoryBus`; instruction fetch, stack access, and implemented extended load/store operations route through that bus.
- Initial `FM7Machine` wrapper defines named development regions for RAM, video RAM, and I/O while sharing the flat bus.
- Automated tests cover CPU operations, subroutine flow, bus byte/word access, and basic machine memory access.
- Host executable named `fum7x360`.

**Hardware-accuracy note:** the named regions in `FM7Machine` are scaffolding, not a verified FM-7 hardware map. They currently use the same flat backing memory; device decoding, correct ROM/RAM banking, the second processor, shared-memory arbitration, interrupts, and memory-mapped devices remain unimplemented. Unsupported CPU opcodes stop execution, so the current subset cannot run FM-7 firmware or commercial software.

## Planned work

1. Verify and implement the real FM-7 memory map and device decoding from reliable technical references.
2. Implement shared-memory coordination and the FM-7 processor arrangement.
3. Expand and validate 6809 instruction coverage, flags, cycle timing, and interrupt handling.
4. Implement video RAM/planes, palette and scanout.
5. Add sound, keyboard/joystick, cassette and disk devices.
6. Integrate the Xbox 360 frontend and select a compatible SDK/toolchain.
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
