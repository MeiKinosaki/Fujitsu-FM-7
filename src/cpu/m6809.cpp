#include "cpu/m6809.h"

namespace fm7 {

M6809::M6809() {
    reset();
}

void M6809::reset(std::uint16_t startAddress) {
    regs_ = Registers{};
    regs_.pc = startAddress;
    cycles_ = 0;
    halted_ = false;
}

std::uint8_t M6809::fetch8() {
    const auto value = memory_[regs_.pc];
    regs_.pc = static_cast<std::uint16_t>(regs_.pc + 1);
    return value;
}

std::uint16_t M6809::fetch16() {
    const auto high = fetch8();
    const auto low = fetch8();
    return static_cast<std::uint16_t>((static_cast<std::uint16_t>(high) << 8) | low);
}

std::uint16_t M6809::read16(std::uint16_t address) const {
    const auto high = memory_[address];
    const auto low = memory_[static_cast<std::uint16_t>(address + 1)];
    return static_cast<std::uint16_t>((static_cast<std::uint16_t>(high) << 8) | low);
}

void M6809::write16(std::uint16_t address, std::uint16_t value) {
    memory_[address] = static_cast<std::uint8_t>(value >> 8);
    memory_[static_cast<std::uint16_t>(address + 1)] = static_cast<std::uint8_t>(value & 0xff);
}

void M6809::setNZ8(std::uint8_t value) {
    regs_.cc = static_cast<std::uint8_t>(regs_.cc & ~(Negative | Zero));
    if (value == 0) regs_.cc = static_cast<std::uint8_t>(regs_.cc | Zero);
    if ((value & 0x80) != 0) regs_.cc = static_cast<std::uint8_t>(regs_.cc | Negative);
}

void M6809::setNZ16(std::uint16_t value) {
    regs_.cc = static_cast<std::uint8_t>(regs_.cc & ~(Negative | Zero));
    if (value == 0) regs_.cc = static_cast<std::uint8_t>(regs_.cc | Zero);
    if ((value & 0x8000) != 0) regs_.cc = static_cast<std::uint8_t>(regs_.cc | Negative);
}

bool M6809::step() {
    if (halted_) return false;

    const auto opcode = fetch8();
    switch (opcode) {
    case 0x12: // NOP
        cycles_ += 2;
        return true;

    case 0x20: { // BRA relative
        const auto displacement = static_cast<std::int8_t>(fetch8());
        regs_.pc = static_cast<std::uint16_t>(regs_.pc + displacement);
        cycles_ += 3;
        return true;
    }

    case 0x39: // RTS (stack semantics not implemented in this initial scaffold)
        halted_ = true;
        cycles_ += 5;
        return true;

    case 0x3F: // SWI: stop until interrupt handling is implemented
        halted_ = true;
        cycles_ += 19;
        return true;

    case 0x4F: // CLRA
        regs_.a = 0;
        regs_.cc = static_cast<std::uint8_t>(regs_.cc & ~(Negative | Zero | Overflow));
        regs_.cc = static_cast<std::uint8_t>(regs_.cc | Zero);
        cycles_ += 2;
        return true;

    case 0x5F: // CLRB
        regs_.b = 0;
        regs_.cc = static_cast<std::uint8_t>(regs_.cc & ~(Negative | Zero | Overflow));
        regs_.cc = static_cast<std::uint8_t>(regs_.cc | Zero);
        cycles_ += 2;
        return true;

    case 0x7E: // JMP extended
        regs_.pc = fetch16();
        cycles_ += 3;
        return true;

    case 0x86: // LDA immediate
        regs_.a = fetch8();
        setNZ8(regs_.a);
        cycles_ += 2;
        return true;

    case 0x8E: // LDX immediate
        regs_.x = fetch16();
        setNZ16(regs_.x);
        cycles_ += 3;
        return true;

    case 0xB6: { // LDA extended
        const auto address = fetch16();
        regs_.a = memory_[address];
        setNZ8(regs_.a);
        cycles_ += 5;
        return true;
    }

    case 0xB7: { // STA extended
        const auto address = fetch16();
        memory_[address] = regs_.a;
        setNZ8(regs_.a);
        regs_.cc = static_cast<std::uint8_t>(regs_.cc & ~Overflow);
        cycles_ += 5;
        return true;
    }

    case 0xC6: // LDB immediate
        regs_.b = fetch8();
        setNZ8(regs_.b);
        cycles_ += 2;
        return true;

    case 0xF6: { // LDB extended
        const auto address = fetch16();
        regs_.b = memory_[address];
        setNZ8(regs_.b);
        cycles_ += 5;
        return true;
    }

    case 0xF7: { // STB extended
        const auto address = fetch16();
        memory_[address] = regs_.b;
        setNZ8(regs_.b);
        regs_.cc = static_cast<std::uint8_t>(regs_.cc & ~Overflow);
        cycles_ += 5;
        return true;
    }

    default:
        halted_ = true;
        return false;
    }
}

} // namespace fm7
