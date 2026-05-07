#pragma once

#include <chrono>

namespace gl {
    class Clock {
    private:
        std::chrono::steady_clock::time_point m_Start{std::chrono::steady_clock::now()};
    
    public:
        float Restart() {
            const float elapsed = GetElapsedTime();
            m_Start = std::chrono::steady_clock::now();
    
            return elapsed;
        }
    
        [[nodiscard]]
        float GetElapsedTime() const {
            return std::chrono::duration<float>(std::chrono::steady_clock::now() - m_Start).count();
        }
    };
}