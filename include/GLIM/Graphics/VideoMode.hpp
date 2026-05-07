#pragma once

#include "GLIM/System/Vector2.hpp"

namespace gl {
    struct VideoMode {
        Vector2u Size;
        unsigned int RefreshRate{0u};

        [[nodiscard]]
        static VideoMode GetDesktopMode();
    };
}