# Fujitsu-FM-7

Experimental Fujitsu FM-7 emulator project targeting Xbox 360.

> **Status:** initial scaffold only. This repository does not yet contain a complete emulator or a working Xbox 360 `default.xex`.

## Goals

- Emulate the original FM-7's two 6809-family CPUs and their coordination.
- Implement main RAM, video RAM, ROM mapping and I/O.
- Reproduce the 640×200 graphics output and 8-color palette.
- Add AY-3-8910 audio, keyboard/joystick input and cassette/disk loading.
- Provide an Xbox 360 frontend and a reproducible build pipeline.

## Development stages

1. Establish a portable C++ core and basic test harness.
2. Implement and test the 6809 CPU core.
3. Implement the FM-7 memory map, shared memory and CPU handshake.
4. Implement video, input, sound and media devices.
5. Integrate the Xbox 360 graphics/input/audio frontend.
6. Build with an Xbox 360-compatible toolchain and test on the target hardware.

## Build the current scaffold

A standard C++17 compiler and CMake are needed for the host-side scaffold:

```sh
cmake -S . -B build
cmake --build build
./build/fm7x360
```

This host build is only a development smoke test; it does **not** produce `default.xex`. Xbox 360 packaging will be added after selecting and validating the target SDK/toolchain.

## References

- [MAME FM-7 driver](https://github.com/mamedev/mame/blob/master/src/mame/fujitsu/fm7.cpp)
- [FM-7 technical notes](https://github.com/0cjs/sedoc/blob/main/8bit/fm7/fm-7.md)

Firmware/BIOS images and commercial software are not included. Use legally obtained files and keep them separate from the source tree.
