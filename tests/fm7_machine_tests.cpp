#include "fm7_machine.h"

#include <cassert>
#include <cstdint>

int main() {
    fm7::FM7Machine machine;

    machine.resetMemory();
    machine.write8(0x1234, 0x5A);
    assert(machine.read8(0x1234) == 0x5A);

    // Named development regions share the same backing bus for now.
    machine.write8(fm7::FM7Machine::kRamStart, 0x11);
    machine.write8(fm7::FM7Machine::kVideoRamStart, 0x22);
    machine.write8(fm7::FM7Machine::kIoStart, 0x33);
    assert(machine.read8(fm7::FM7Machine::kRamStart) == 0x11);
    assert(machine.read8(fm7::FM7Machine::kVideoRamStart) == 0x22);
    assert(machine.read8(fm7::FM7Machine::kIoStart) == 0x33);

    return 0;
}
