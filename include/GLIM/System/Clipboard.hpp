#pragma once

#include <string>

namespace gl {
    namespace Clipboard {
        extern bool CopyString(const std::string& string);
        extern std::string GetString();
    }
}
