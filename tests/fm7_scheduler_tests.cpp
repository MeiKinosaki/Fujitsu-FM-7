#include "fm7_scheduler.h"

#include <cassert>
#include <cstdint>

struct Counter {
    std::uint32_t cyclesPerStep;
    std::uint32_t calls = 0;
};

static std::uint32_t runStep(void* context) {
    auto& counter = *static_cast<Counter*>(context);
    ++counter.calls;
    return counter.cyclesPerStep;
}

int main() {
    Counter mainCpu{4};
    Counter subCpu{2};
    fm7::FM7Scheduler scheduler;
    scheduler.attachMain(&runStep, &mainCpu);
    scheduler.attachSubsystem(&runStep, &subCpu);
    scheduler.setSubsystemRatio(1, 1);

    assert(scheduler.step());
    assert(mainCpu.calls == 1);
    assert(subCpu.calls == 2);
    assert(scheduler.mainSlot().cycles == 4);
    assert(scheduler.subsystemSlot().cycles == 4);

    scheduler.setSubsystemRatio(1, 2);
    assert(scheduler.step());
    assert(mainCpu.calls == 2);
    assert(subCpu.calls == 3);
    assert(scheduler.mainSlot().cycles == 8);
    assert(scheduler.subsystemSlot().cycles == 6);

    scheduler.reset();
    assert(scheduler.mainSlot().cycles == 0);
    assert(scheduler.subsystemSlot().cycles == 0);
    return 0;
}
