#pragma once

#include "cpu/m6809.h"
#include "fm7_scheduler.h"

#include <cstdint>
#include <limits>

namespace fm7 {

// Adapter lets FM7Scheduler run an M6809 without coupling the scheduler to
// CPU internals. The context passed to attachMain/attachSubsystem is M6809*.
inline std::uint32_t stepM6809(void* context) {
    if (context == nullptr) return 0;
    auto& cpu = *static_cast<M6809*>(context);
    const auto before = cpu.cycles();
    if (!cpu.step()) return 0;
    const auto elapsed = cpu.cycles() - before;
    if (elapsed > std::numeric_limits<std::uint32_t>::max()) {
        return std::numeric_limits<std::uint32_t>::max();
    }
    return static_cast<std::uint32_t>(elapsed);
}

} // namespace fm7
