#pragma once

#include "memory_bus.h"

#include <array>
#include <cstdint>

namespace fm7 {

// FM-7 address-map development scaffold. Constants are based on the current
// reference map, while ROM banking and peripheral decoding remain TODOs.
// The class implements the main CPU's memory interface so CPU accesses can be
// routed through the machine-level shared-RAM decoder.
class FM7Machine final : public MemoryInterface {
public:
    static constexpr std::uint16_t kMainRamStart = 0x0000;
    static constexpr std::uint16_t kMainRamEnd = 0x7FFF;
    static constexpr std::uint16_t kMainBankedStart = 0x8000;
    static constexpr std::uint16_t kMainBankedEnd = 0xFBFF;
    static constexpr std::uint16_t kMainLowRamStart = 0xFC00;
    static constexpr std::uint16_t kMainLowRamEnd = 0xFC7F;
    static constexpr std::uint16_t kSharedRamStart = 0xFC80;
    static constexpr std::uint16_t kSharedRamEnd = 0xFCFF;
    static constexpr std::uint16_t kMainIoStart = 0xFD00;
    static constexpr std::uint16_t kMainIoEnd = 0xFDFF;
    static constexpr std::uint16_t kBootRomStart = 0xFE00;
    static constexpr std::uint16_t kBootRomEnd = 0xFFDF;

    static constexpr std::uint16_t kSubVramStart = 0x0000;
    static constexpr std::uint16_t kSubVramEnd = 0xBFFF;
    static constexpr std::uint16_t kSubConsoleRamStart = 0xC000;
    static constexpr std::uint16_t kSubConsoleRamEnd = 0xCFFF;
    static constexpr std::uint16_t kSubWorkRamStart = 0xD000;
    static constexpr std::uint16_t kSubWorkRamEnd = 0xD37F;
    static constexpr std::uint16_t kSubSharedRamStart = 0xD380;
    static constexpr std::uint16_t kSubSharedRamEnd = 0xD3FF;
    static constexpr std::uint16_t kSubIoStart = 0xD400;
    static constexpr std::uint16_t kSubIoEnd = 0xD4FF;
    static constexpr std::uint16_t kSubRomStart = 0xD800;
    static constexpr std::uint16_t kSubRomEnd = 0xFFFF;

    FM7Machine() = default;

    MemoryBus& mainBus() noexcept { return mainBus_; }
    const MemoryBus& mainBus() const noexcept { return mainBus_; }
    MemoryBus& subBus() noexcept { return subBus_; }
    const MemoryBus& subBus() const noexcept { return subBus_; }

    std::uint8_t read8(std::uint16_t address) const noexcept override {
        return mainRead8(address);
    }

    void write8(std::uint16_t address, std::uint8_t value) noexcept override {
        mainWrite8(address, value);
    }

    std::uint8_t mainRead8(std::uint16_t address) const noexcept {
        if (address >= kSharedRamStart && address <= kSharedRamEnd) {
            return sharedRam_[address - kSharedRamStart];
        }
        return mainBus_.read8(address);
    }

    void mainWrite8(std::uint16_t address, std::uint8_t value) noexcept {
        if (address >= kSharedRamStart && address <= kSharedRamEnd) {
            sharedRam_[address - kSharedRamStart] = value;
            return;
        }
        mainBus_.write8(address, value);
    }

    std::uint8_t subRead8(std::uint16_t address) const noexcept {
        if (address >= kSubSharedRamStart && address <= kSubSharedRamEnd) {
            return sharedRam_[address - kSubSharedRamStart];
        }
        return subBus_.read8(address);
    }

    void subWrite8(std::uint16_t address, std::uint8_t value) noexcept {
        if (address >= kSubSharedRamStart && address <= kSubSharedRamEnd) {
            sharedRam_[address - kSubSharedRamStart] = value;
            return;
        }
        subBus_.write8(address, value);
    }

    void resetMemory(std::uint8_t fill = 0) noexcept {
        mainBus_.clear(fill);
        subBus_.clear(fill);
        sharedRam_.fill(fill);
    }

private:
    MemoryBus mainBus_{};
    MemoryBus subBus_{};
    std::array<std::uint8_t, 0x80> sharedRam_{};
};

} // namespace fm7
