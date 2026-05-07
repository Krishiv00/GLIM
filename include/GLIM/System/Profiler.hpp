#pragma once

#include "GLIM/System/Clock.hpp"

namespace gl {
    class Profiler final {
    private:
        Clock m_Clock;
        float m_Interval{1.f};
        unsigned int m_Frames{0};
        float m_AvgFPS{0.f};

    public:
        Profiler() = default;
        explicit Profiler(float interval) : m_Interval(interval) {}

        bool Tick() {
            ++m_Frames;

            const float elapsed = m_Clock.GetElapsedTime();

            if (elapsed >= m_Interval) {
                m_AvgFPS = static_cast<float>(m_Frames) / elapsed;
                m_Frames = 0;
                m_Clock.Restart();

                return true;
            }

            return false;
        }

        [[nodiscard]]
        inline unsigned int GetAverageFPS() const noexcept {
            return static_cast<unsigned int>(m_AvgFPS);
        }
    };
}