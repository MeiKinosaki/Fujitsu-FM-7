#include "memory_bus.h"

#include <cassert>
#include <cstdint>

int main() {
    fm7::MemoryBus bus;

    bus.write8(0x1234, 0xA5);
    assert(bus.read8(0x1234) == 0xA5);

    bus.write16(0x2000, 0xBEEF);
    assert(bus.read8(0x2000) == 0xBE);
    assert(bus.read8(0x2001) == 0xEF);
    assert(bus.read16(0x2000) == 0xBEEF);

    // Address arithmetic wraps around the 16-bit address space.
    bus.write16(0xFFFF, 0x1234);
    assert(bus.read8(0xFFFF) == 0x12);
    assert(bus.read8(0x0000) == 0x34);
    assert(bus.read16(0xFFFF) == 0x1234);

    bus.clear(0xCC);
    assert(bus.read8(0x1234) == 0xCC);
    assert(bus.read8(0xFFFF) == 0xCC);

    return 0;
}
