#include "cpu/m6809.h"

#include <iostream>

int main() {
    fm7::M6809 cpu;
    auto& memory = cpu.memory();

    // Tiny smoke-test program: LDA #$42; STA $2000; NOP.
    memory[0x0000] = 0x86;
    memory[0x0001] = 0x42;
    memory[0x0002] = 0xB7;
    memory[0x0003] = 0x20;
    memory[0x0004] = 0x00;
    memory[0x0005] = 0x12;

    cpu.reset(0x0000);
    for (int i = 0; i < 3 && !cpu.halted(); ++i) {
        if (!cpu.step()) {
            std::cerr << "Unsupported opcode encountered.\n";
            return 1;
        }
    }

    if (memory[0x2000] != 0x42) {
        std::cerr << "Fum7X360 CPU smoke test failed.\n";
        return 1;
    }

    std::cout << "Fum7X360 host smoke test passed; stored value 0x42 at $2000.\n";
    std::cout << "This host executable is not an Xbox 360 default.xex.\n";
    return 0;
}
