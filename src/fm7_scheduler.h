#pragma once

#include <cstdint>

namespace fm7 {

// Deterministic cooperative scheduler scaffold. The FM-7 hardware timing ratio
// must be verified against primary documentation before using this for fidelity.
// Callbacks execute one CPU step and return the number of cycles actually used.
class FM7Scheduler {
public:
    using StepFunction = std::uint32_t (*)(void* context);

    struct CpuSlot {
        StepFunction step = nullptr;
        void* context = nullptr;
        std::uint64_t cycles = 0;
        bool enabled = true;
    };

    void attachMain(StepFunction step, void* context) noexcept {
        main_ = CpuSlot{step, context, 0, true};
    }

    void attachSubsystem(StepFunction step, void* context) noexcept {
        subsystem_ = CpuSlot{step, context, 0, true};
    }

    void setSubsystemRatio(std::uint32_t numerator,
                           std::uint32_t denominator) noexcept {
        if (numerator == 0 || denominator == 0) return;
        ratioNumerator_ = numerator;
        ratioDenominator_ = denominator;
        ratioRemainder_ = 0;
    }

    // Run one main-CPU step and enough subsystem steps to approach the configured
    // cycle ratio. Returns false if no attached, enabled CPU made progress.
    bool step() noexcept {
        bool progressed = false;
        if (main_.enabled && main_.step != nullptr) {
            const auto used = main_.step(main_.context);
            main_.cycles += used;
            progressed = progressed || used != 0;
            ratioRemainder_ += static_cast<std::uint64_t>(used) * ratioNumerator_;
        }

        const std::uint64_t target = ratioRemainder_ / ratioDenominator_;
        ratioRemainder_ %= ratioDenominator_;
        std::uint64_t subsystemBudget = target;
        while (subsystem_.enabled && subsystem_.step != nullptr &&
               subsystemBudget > 0) {
            const auto used = subsystem_.step(subsystem_.context);
            if (used == 0) break;
            subsystem_.cycles += used;
            progressed = true;
            if (used >= subsystemBudget) subsystemBudget = 0;
            else subsystemBudget -= used;
        }
        return progressed;
    }

    void reset() noexcept {
        main_.cycles = 0;
        subsystem_.cycles = 0;
        ratioRemainder_ = 0;
    }

    const CpuSlot& mainSlot() const noexcept { return main_; }
    const CpuSlot& subsystemSlot() const noexcept { return subsystem_; }
    std::uint32_t ratioNumerator() const noexcept { return ratioNumerator_; }
    std::uint32_t ratioDenominator() const noexcept { return ratioDenominator_; }

private:
    CpuSlot main_{};
    CpuSlot subsystem_{};
    std::uint32_t ratioNumerator_ = 1;
    std::uint32_t ratioDenominator_ = 1;
    std::uint64_t ratioRemainder_ = 0;
};

} // namespace fm7
