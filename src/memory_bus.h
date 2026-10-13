#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace fm7 {

// Initial 64 KiB flat bus. Device mapping and shared-memory arbitration will
// be layered on later; this class intentionally defines the access boundary.
class MemoryBus {
public:
    static constexpr std::size_t kAddressSpaceSize = 65536;

    MemoryBus() = default;

    std::uint8_t read8(std::uint16_t address) const noexcept {
        return memory_[address];
    }

    void write8(std::uint16_t address, std::uint8_t value) noexcept {
        memory_[address] = value;
    }

    std::uint16_t read16(std::uint16_t address) const noexcept {
        const auto high = read8(address);
        const auto low = read8(static_cast<std::uint16_t>(address + 1));
        return static_cast<std::uint16_t>((static_cast<std::uint16_t>(high) << 8) | low);
    }

    void write16(std::uint16_t address, std::uint16_t value) noexcept {
        write8(address, static_cast<std::uint8_t>(value >> 8));
        write8(static_cast<std::uint16_t>(address + 1), static_cast<std::uint8_t>(value));
    }

    void clear(std::uint8_t fill = 0) noexcept {
        memory_.fill(fill);
    }

    std::array<std::uint8_t, kAddressSpaceSize>& rawMemory() noexcept {
        return memory_;
    }

    const std::array<std::uint8_t, kAddressSpaceSize>& rawMemory() const noexcept {
        return memory_;
    }

private:
    std::array<std::uint8_t, kAddressSpaceSize> memory_{};
};

} // namespace fm7
