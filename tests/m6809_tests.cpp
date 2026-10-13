#include "cpu/m6809.h"

#include <cassert>
#include <cstdint>

int main() {
    {
        fm7::M6809 cpu;
        auto& mem = cpu.memory();
        mem[0x0100] = 0x86; // LDA #$7f
        mem[0x0101] = 0x7f;
        mem[0x0102] = 0xB7; // STA $2000
        mem[0x0103] = 0x20;
        mem[0x0104] = 0x00;
        cpu.reset(0x0100);

        assert(cpu.step());
        assert(cpu.registers().a == 0x7f);
        assert(cpu.step());
        assert(mem[0x2000] == 0x7f);
        assert(cpu.cycles() == 7);
    }

    {
        fm7::M6809 cpu;
        auto& mem = cpu.memory();
        mem[0x0200] = 0x20; // BRA +2
        mem[0x0201] = 0x02;
        mem[0x0204] = 0xC6; // LDB #$33
        mem[0x0205] = 0x33;
        cpu.reset(0x0200);

        assert(cpu.step());
        assert(cpu.registers().pc == 0x0204);
        assert(cpu.step());
        assert(cpu.registers().b == 0x33);
    }

    {
        fm7::M6809 cpu;
        cpu.memory()[0] = 0x00; // deliberately unsupported
        cpu.reset(0);
        assert(!cpu.step());
        assert(cpu.halted());
    }

    return 0;
}
