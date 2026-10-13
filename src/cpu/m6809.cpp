#include "cpu/m6809.h"

namespace fm7 {

M6809::M6809() { reset(); }

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
    memory_[static_cast<std::uint16_t>(address + 1)] = static_cast<std::uint8_t>(value);
}

void M6809::push8(std::uint16_t& stack, std::uint8_t value) {
    stack = static_cast<std::uint16_t>(stack - 1);
    memory_[stack] = value;
}

std::uint8_t M6809::pull8(std::uint16_t& stack) {
    const auto value = memory_[stack];
    stack = static_cast<std::uint16_t>(stack + 1);
    return value;
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

void M6809::setAddFlags8(std::uint8_t lhs, std::uint8_t rhs, std::uint16_t result) {
    const auto value = static_cast<std::uint8_t>(result);
    regs_.cc = static_cast<std::uint8_t>(regs_.cc & ~(HalfCarry | Negative | Zero | Overflow | Carry));
    if (((lhs & 0x0f) + (rhs & 0x0f)) > 0x0f) regs_.cc |= HalfCarry;
    if (value == 0) regs_.cc |= Zero;
    if (value & 0x80) regs_.cc |= Negative;
    if ((~(lhs ^ rhs) & (lhs ^ value) & 0x80) != 0) regs_.cc |= Overflow;
    if (result & 0x100) regs_.cc |= Carry;
}

void M6809::setSubFlags8(std::uint8_t lhs, std::uint8_t rhs, std::uint16_t result) {
    const auto value = static_cast<std::uint8_t>(result);
    regs_.cc = static_cast<std::uint8_t>(regs_.cc & ~(Negative | Zero | Overflow | Carry));
    if (value == 0) regs_.cc |= Zero;
    if (value & 0x80) regs_.cc |= Negative;
    if (((lhs ^ rhs) & (lhs ^ value) & 0x80) != 0) regs_.cc |= Overflow;
    if (lhs < rhs) regs_.cc |= Carry;
}

bool M6809::branchCondition(std::uint8_t opcode) const {
    const bool c = (regs_.cc & Carry) != 0;
    const bool z = (regs_.cc & Zero) != 0;
    const bool v = (regs_.cc & Overflow) != 0;
    const bool n = (regs_.cc & Negative) != 0;
    switch (opcode) {
    case 0x21: return true;        // BRN is never taken (not BRA)
    case 0x22: return !c && !z;    // BHI
    case 0x23: return c || z;      // BLS
    case 0x24: return !c;          // BCC
    case 0x25: return c;           // BCS
    case 0x26: return !z;          // BNE
    case 0x27: return z;           // BEQ
    case 0x28: return !v;          // BVC
    case 0x29: return v;           // BVS
    case 0x2A: return !n;          // BPL
    case 0x2B: return n;           // BMI
    case 0x2C: return n == v;      // BGE
    case 0x2D: return n != v;      // BLT
    case 0x2E: return !z && n == v;// BGT
    case 0x2F: return z || n != v; // BLE
    default: return false;
    }
}

bool M6809::step() {
    if (halted_) return false;
    const auto opcode = fetch8();
    switch (opcode) {
    case 0x12: cycles_ += 2; return true; // NOP

    case 0x16: { // LBRA, signed 16-bit displacement
        const auto displacement = static_cast<std::int16_t>(fetch16());
        regs_.pc = static_cast<std::uint16_t>(regs_.pc + displacement);
        cycles_ += 5;
        return true;
    }

    case 0x20: { // BRA
        const auto displacement = static_cast<std::int8_t>(fetch8());
        regs_.pc = static_cast<std::uint16_t>(regs_.pc + displacement);
        cycles_ += 3;
        return true;
    }

    case 0x21: case 0x22: case 0x23: case 0x24: case 0x25:
    case 0x26: case 0x27: case 0x28: case 0x29: case 0x2A:
    case 0x2B: case 0x2C: case 0x2D: case 0x2E: case 0x2F: {
        const auto displacement = static_cast<std::int8_t>(fetch8());
        if (branchCondition(opcode)) regs_.pc = static_cast<std::uint16_t>(regs_.pc + displacement);
        cycles_ += 3;
        return true;
    }

    case 0x32: // LEAS ,Y is not the encoding; do not implement as an alias.
        halted_ = true;
        return false;

    case 0x34: { // PSHS: push selected registers in 6809 order
        const auto mask = fetch8();
        if (mask & 0x80) push8(regs_.s, static_cast<std::uint8_t>(regs_.pc));
        if (mask & 0x80) push8(regs_.s, static_cast<std::uint8_t>(regs_.pc >> 8));
        if (mask & 0x40) push8(regs_.s, static_cast<std::uint8_t>(regs_.u));
        if (mask & 0x40) push8(regs_.s, static_cast<std::uint8_t>(regs_.u >> 8));
        if (mask & 0x20) push8(regs_.s, static_cast<std::uint8_t>(regs_.y));
        if (mask & 0x20) push8(regs_.s, static_cast<std::uint8_t>(regs_.y >> 8));
        if (mask & 0x10) push8(regs_.s, static_cast<std::uint8_t>(regs_.x));
        if (mask & 0x10) push8(regs_.s, static_cast<std::uint8_t>(regs_.x >> 8));
        if (mask & 0x08) push8(regs_.s, regs_.dp);
        if (mask & 0x04) push8(regs_.s, regs_.b);
        if (mask & 0x02) push8(regs_.s, regs_.a);
        if (mask & 0x01) push8(regs_.s, regs_.cc);
        cycles_ += 5;
        return true;
    }

    case 0x35: { // PULS
        const auto mask = fetch8();
        if (mask & 0x01) regs_.cc = pull8(regs_.s);
        if (mask & 0x02) regs_.a = pull8(regs_.s);
        if (mask & 0x04) regs_.b = pull8(regs_.s);
        if (mask & 0x08) regs_.dp = pull8(regs_.s);
        if (mask & 0x10) regs_.x = static_cast<std::uint16_t>((pull8(regs_.s) << 8) | pull8(regs_.s));
        if (mask & 0x20) regs_.y = static_cast<std::uint16_t>((pull8(regs_.s) << 8) | pull8(regs_.s));
        if (mask & 0x40) regs_.u = static_cast<std::uint16_t>((pull8(regs_.s) << 8) | pull8(regs_.s));
        if (mask & 0x80) regs_.pc = static_cast<std::uint16_t>((pull8(regs_.s) << 8) | pull8(regs_.s));
        cycles_ += 5;
        return true;
    }

    case 0x39: // RTS
        regs_.pc = read16(regs_.s);
        regs_.s = static_cast<std::uint16_t>(regs_.s + 2);
        cycles_ += 5;
        return true;

    case 0x3F: // SWI requires vector and interrupt-stack behavior; not yet implemented
        halted_ = true;
        return false;

    case 0x4F: // CLRA
        regs_.a = 0;
        regs_.cc = static_cast<std::uint8_t>((regs_.cc & ~(Negative | Overflow | Zero)) | Zero);
        cycles_ += 2;
        return true;

    case 0x5F: // CLRB
        regs_.b = 0;
        regs_.cc = static_cast<std::uint8_t>((regs_.cc & ~(Negative | Overflow | Zero)) | Zero);
        cycles_ += 2;
        return true;

    case 0x7E: regs_.pc = fetch16(); cycles_ += 3; return true; // JMP extended

    case 0x80: { // SUBA immediate
        const auto rhs = fetch8();
        const auto lhs = regs_.a;
        const auto result = static_cast<std::uint16_t>(lhs - rhs);
        regs_.a = static_cast<std::uint8_t>(result);
        setSubFlags8(lhs, rhs, result);
        cycles_ += 2;
        return true;
    }
    case 0x81: { // CMPA immediate
        const auto rhs = fetch8();
        setSubFlags8(regs_.a, rhs, static_cast<std::uint16_t>(regs_.a - rhs));
        cycles_ += 2;
        return true;
    }
    case 0x8B: { // ADDA immediate
        const auto rhs = fetch8();
        const auto lhs = regs_.a;
        const auto result = static_cast<std::uint16_t>(lhs + rhs);
        regs_.a = static_cast<std::uint8_t>(result);
        setAddFlags8(lhs, rhs, result);
        cycles_ += 2;
        return true;
    }
    case 0x84: regs_.a &= fetch8(); setNZ8(regs_.a); regs_.cc &= static_cast<std::uint8_t>(~Overflow); cycles_ += 2; return true; // ANDA
    case 0x8A: regs_.a |= fetch8(); setNZ8(regs_.a); regs_.cc &= static_cast<std::uint8_t>(~Overflow); cycles_ += 2; return true; // ORA
    case 0x88: regs_.a ^= fetch8(); setNZ8(regs_.a); regs_.cc &= static_cast<std::uint8_t>(~Overflow); cycles_ += 2; return true; // EORA

    case 0x86: regs_.a = fetch8(); setNZ8(regs_.a); regs_.cc &= static_cast<std::uint8_t>(~Overflow); cycles_ += 2; return true; // LDA immediate
    case 0x8E: regs_.x = fetch16(); setNZ16(regs_.x); regs_.cc &= static_cast<std::uint8_t>(~Overflow); cycles_ += 3; return true; // LDX immediate
    case 0xC6: regs_.b = fetch8(); setNZ8(regs_.b); regs_.cc &= static_cast<std::uint8_t>(~Overflow); cycles_ += 2; return true; // LDB immediate
    case 0xCE: regs_.u = fetch16(); setNZ16(regs_.u); regs_.cc &= static_cast<std::uint8_t>(~Overflow); cycles_ += 3; return true; // LDU immediate

    case 0xB6: { const auto address = fetch16(); regs_.a = memory_[address]; setNZ8(regs_.a); regs_.cc &= static_cast<std::uint8_t>(~Overflow); cycles_ += 5; return true; }
    case 0xB7: { const auto address = fetch16(); memory_[address] = regs_.a; setNZ8(regs_.a); regs_.cc &= static_cast<std::uint8_t>(~Overflow); cycles_ += 5; return true; }
    case 0xF6: { const auto address = fetch16(); regs_.b = memory_[address]; setNZ8(regs_.b); regs_.cc &= static_cast<std::uint8_t>(~Overflow); cycles_ += 5; return true; }
    case 0xF7: { const auto address = fetch16(); memory_[address] = regs_.b; setNZ8(regs_.b); regs_.cc &= static_cast<std::uint8_t>(~Overflow); cycles_ += 5; return true; }

    case 0xC0: { const auto rhs = fetch8(); const auto lhs = regs_.b; const auto result = static_cast<std::uint16_t>(lhs - rhs); regs_.b = static_cast<std::uint8_t>(result); setSubFlags8(lhs, rhs, result); cycles_ += 2; return true; } // SUBB
    case 0xC1: setSubFlags8(regs_.b, fetch8(), static_cast<std::uint16_t>(regs_.b - memory_[static_cast<std::uint16_t>(regs_.pc - 1)])); cycles_ += 2; return true; // CMPB immediate
    case 0xCB: { const auto rhs = fetch8(); const auto lhs = regs_.b; const auto result = static_cast<std::uint16_t>(lhs + rhs); regs_.b = static_cast<std::uint8_t>(result); setAddFlags8(lhs, rhs, result); cycles_ += 2; return true; } // ADDB

    default:
        halted_ = true;
        return false;
    }
}

} // namespace fm7
