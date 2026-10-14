#pragma once

#include "memory_bus.h"

#include <cstdint>

namespace fm7 {

// Explicitly an initial development map, not a complete hardware-accurate FM-7
// decoder. It establishes named regions so verified hardware mapping can be
// added without changing CPU-facing memory APIs.
class FM7Machine {
public:
    static constexpr std::uint16_t kRamStart = 0x0000;
    static constexpr std::uint16_t kRamEnd = 0x7FFF;
    static constexpr std::uint16_t kVideoRamStart = 0x8000;
    static constexpr std::uint16_t kVideoRamEnd = 0xBFFF;
    static constexpr std::uint16_t kIoStart = 0xD000;
    static constexpr std::uint16_t kIoEnd = 0xDFFF;

    FM7Machine() = default;

    MemoryBus& bus() noexcept { return bus_; }
    const MemoryBus& bus() const noexcept { return bus_; }

    std::uint8_t read8(std::uint16_t address) const noexcept {
        return bus_.read8(address);
    }

    void write8(std::uint16_t address, std::uint8_t value) noexcept {
        bus_.write8(address, value);
    }

    void resetMemory(std::uint8_t fill = 0) noexcept {
        bus_.clear(fill);
    }

private:
    MemoryBus bus_{};
};

} // namespace fm7
