#pragma once

#include "memory_bus.h"

#include <cstddef>
#include <cstdint>

namespace fm7 {

class M6809 {
public:
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
        Carry = 0x01, Overflow = 0x02, Zero = 0x04, Negative = 0x08,
        InterruptMask = 0x10, HalfCarry = 0x20, FirqMask = 0x40, EntireState = 0x80
    };

    M6809();
    explicit M6809(MemoryBus& bus);
    void reset(std::uint16_t startAddress = 0x0000);
    bool step();

    Registers& registers() noexcept { return regs_; }
    const Registers& registers() const noexcept { return regs_; }
    MemoryBus& bus() noexcept { return *bus_; }
    const MemoryBus& bus() const noexcept { return *bus_; }
    std::uint64_t cycles() const noexcept { return cycles_; }
    bool halted() const noexcept { return halted_; }

private:
    std::uint8_t fetch8();
    std::uint16_t fetch16();
    std::uint16_t read16(std::uint16_t address) const;
    void write16(std::uint16_t address, std::uint16_t value);
    void push8(std::uint16_t& stack, std::uint8_t value);
    std::uint8_t pull8(std::uint16_t& stack);
    void push16(std::uint16_t& stack, std::uint16_t value);
    std::uint16_t pull16(std::uint16_t& stack);
    void setNZ8(std::uint8_t value);
    void setNZ16(std::uint16_t value);
    void setAddFlags8(std::uint8_t lhs, std::uint8_t rhs, std::uint16_t result);
    void setSubFlags8(std::uint8_t lhs, std::uint8_t rhs, std::uint16_t result);
    bool branchCondition(std::uint8_t opcode) const;

    MemoryBus ownedBus_{};
    MemoryBus* bus_ = &ownedBus_;
    Registers regs_{};
    std::uint64_t cycles_ = 0;
    bool halted_ = false;
};

} // namespace fm7
