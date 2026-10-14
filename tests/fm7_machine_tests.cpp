#include "fm7_machine.h"

#include <cassert>
#include <cstdint>

int main() {
    fm7::FM7Machine machine;
    machine.resetMemory();

    // Ordinary RAM is isolated between the two address spaces.
    machine.mainWrite8(0x1234, 0x11);
    machine.subWrite8(0x1234, 0x22);
    assert(machine.mainRead8(0x1234) == 0x11);
    assert(machine.subRead8(0x1234) == 0x22);

    // Main CPU and subsystem CPU expose the same physical shared byte
    // at different addresses in their respective address spaces.
    machine.mainWrite8(fm7::FM7Machine::kSharedRamStart, 0x5A);
    assert(machine.subRead8(fm7::FM7Machine::kSubSharedRamStart) == 0x5A);
    machine.subWrite8(fm7::FM7Machine::kSubSharedRamStart + 1, 0xC3);
    assert(machine.mainRead8(fm7::FM7Machine::kSharedRamStart + 1) == 0xC3);

    machine.resetMemory(0xA5);
    assert(machine.mainRead8(0x1234) == 0xA5);
    assert(machine.subRead8(0x1234) == 0xA5);
    assert(machine.mainRead8(fm7::FM7Machine::kSharedRamStart) == 0xA5);
    assert(machine.subRead8(fm7::FM7Machine::kSubSharedRamStart) == 0xA5);

    return 0;
}
