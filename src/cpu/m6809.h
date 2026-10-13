#pragma once

#include <array>
#include <cstdint>

namespace fm7 {

class M6809 {
public:
    static constexpr std::size_t kMemorySize = 65536;

    struct Registers {
        std::uint16_t pc = 0;
        std::uint16_t x = 0;
        std::uint16_t y = 0;
        std::uint16_t u = 0;
        std::uint16_t s = 0;
        std::uint8_t a = 0;
        std::uint8_t b = 0;
        std::uint8_t dp = 0;
        std::uint8_t cc = 0;
    };

    enum ConditionCode : std::uint8_t {
        Carry = 0x01,
        Overflow = 0x02,
        Zero = 0x04,
        Negative = 0x08,
        InterruptMask = 0x10,
        HalfCarry = 0x20,
        FirqMask = 0x40,
        EntireState = 0x80
    };

    M6809();

    void reset(std::uint16_t startAddress = 0x0000);
    // Executes one instruction from the currently implemented subset.
    // Returns false for an unsupported opcode.
    bool step();

    Registers& registers() noexcept { return regs_; }
    const Registers& registers() const noexcept { return regs_; }
    std::array<std::uint8_t, kMemorySize>& memory() noexcept { return memory_; }
    const std::array<std::uint8_t, kMemorySize>& memory() const noexcept { return memory_; }
    std::uint64_t cycles() const noexcept { return cycles_; }
    bool halted() const noexcept { return halted_; }

private:
    std::uint8_t fetch8();
    std::uint16_t fetch16();
    std::uint16_t read16(std::uint16_t address) const;
    void write16(std::uint16_t address, std::uint16_t value);
    void setNZ8(std::uint8_t value);
    void setNZ16(std::uint16_t value);

    Registers regs_{};
    std::array<std::uint8_t, kMemorySize> memory_{};
    std::uint64_t cycles_ = 0;
    bool halted_ = false;
};

} // namespace fm7
