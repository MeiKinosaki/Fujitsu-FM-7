#include "cpu/m6809.h"
#include "fm7_cpu_runner.h"
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
    Counter mainCounter{4};
    Counter subCounter{2};
    fm7::FM7Scheduler scheduler;
    scheduler.attachMain(&runStep, &mainCounter);
    scheduler.attachSubsystem(&runStep, &subCounter);
    scheduler.setSubsystemRatio(1, 1);

    assert(scheduler.step());
    assert(mainCounter.calls == 1);
    assert(subCounter.calls == 2);
    assert(scheduler.mainSlot().cycles == 4);
    assert(scheduler.subsystemSlot().cycles == 4);

    scheduler.setSubsystemRatio(1, 2);
    assert(scheduler.step());
    assert(mainCounter.calls == 2);
    assert(subCounter.calls == 3);
    assert(scheduler.mainSlot().cycles == 8);
    assert(scheduler.subsystemSlot().cycles == 6);

    scheduler.reset();
    assert(scheduler.mainSlot().cycles == 0);
    assert(scheduler.subsystemSlot().cycles == 0);

    // Smoke-test the real CPU adapter with two independent 6809 instances.
    fm7::MemoryBus mainBus;
    fm7::MemoryBus subBus;
    mainBus.write8(0x0100, 0x12); // NOP
    mainBus.write8(0x0101, 0x12);
    subBus.write8(0x0200, 0x12);  // NOP
    subBus.write8(0x0201, 0x12);
    fm7::M6809 mainCpu(mainBus);
    fm7::M6809 subCpu(subBus);
    mainCpu.reset(0x0100);
    subCpu.reset(0x0200);

    fm7::FM7Scheduler cpuScheduler;
    cpuScheduler.attachMain(&fm7::stepM6809, &mainCpu);
    cpuScheduler.attachSubsystem(&fm7::stepM6809, &subCpu);
    cpuScheduler.setSubsystemRatio(1, 1);
    assert(cpuScheduler.step());
    assert(mainCpu.registers().pc == 0x0101);
    assert(subCpu.registers().pc == 0x0201);
    assert(cpuScheduler.mainSlot().cycles == 2);
    assert(cpuScheduler.subsystemSlot().cycles == 2);
    return 0;
}
