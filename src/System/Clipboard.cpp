#include "GLIM/System/Clipboard.hpp"

#if defined(_WIN32)

#pragma region Windows

#include <windows.h>

bool gl::Clipboard::CopyString(const std::string& string) {
    if (!OpenClipboard(nullptr)) {
        return false;
    }

    if (!EmptyClipboard()) {
        CloseClipboard();
        return false;
    }

    int wideLen = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, nullptr, 0);
    if (wideLen <= 0) {
        CloseClipboard();
        return false;
    }

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, static_cast<SIZE_T>(wideLen) * sizeof(wchar_t));
    if (!hMem) {
        CloseClipboard();
        return false;
    }

    wchar_t* dest = static_cast<wchar_t*>(GlobalLock(hMem));
    if (!dest) {
        GlobalFree(hMem);
        CloseClipboard();
        return false;
    }

    MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, dest, wideLen);
    GlobalUnlock(hMem);

    if (!SetClipboardData(CF_UNICODETEXT, hMem)) {
        GlobalFree(hMem);
        CloseClipboard();
        return false;
    }

    CloseClipboard();
    return true;
}

std::string gl::Clipboard::GetString() {
    if (!IsClipboardFormatAvailable(CF_UNICODETEXT)) {
        return std::string();
    }

    if (!OpenClipboard(nullptr)) {
        return std::string();
    }

    HGLOBAL hMem = GetClipboardData(CF_UNICODETEXT);
    if (!hMem) {
        CloseClipboard();
        return std::string();
    }

    const wchar_t* wideStr = static_cast<const wchar_t*>(GlobalLock(hMem));
    if (!wideStr) {
        CloseClipboard();
        return std::string();
    }

    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, nullptr, 0, nullptr, nullptr);
    std::string result;
    if (utf8Len > 0) {
        result.resize(static_cast<size_t>(utf8Len) - 1);
        WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, result.data(), utf8Len, nullptr, nullptr);
    }

    GlobalUnlock(hMem);
    CloseClipboard();
    
    return result;
}

#elif defined(__APPLE__)

#pragma region macOS

#include <cstdio>
#include <cstdlib>
#include <array>

static std::string RunCommand(const char* cmd, const std::string* stdinData, bool* ok) {
    *ok = false;

    FILE* pipe = popen(cmd, stdinData ? "w" : "r");
    if (!pipe) {
        return std::string();
    }

    std::string result;
    if (stdinData) {
        fwrite(stdinData->data(), 1, stdinData->size(), pipe);
    } else {
        std::array<char, 4096> buffer{};
        size_t bytesRead = 0;
        while ((bytesRead = fread(buffer.data(), 1, buffer.size(), pipe)) > 0) {
            result.append(buffer.data(), bytesRead);
        }
    }

    int status = pclose(pipe);
    *ok = (status == 0);
    return result;
}

bool gl::Clipboard::CopyString(const std::string& string) {
    bool ok = false;
    RunCommand("pbcopy", &string, &ok);
    return ok;
}

std::string gl::Clipboard::GetString() {
    bool ok = false;
    std::string result = RunCommand("pbpaste", nullptr, &ok);
    return ok ? result : std::string();
}

#elif defined(__linux__)

#pragma region Linux

#include <cstdio>
#include <cstdlib>
#include <array>

static std::string RunCommand(const char* cmd, const std::string* stdinData, bool* ok) {
    *ok = false;

    FILE* pipe = popen(cmd, stdinData ? "w" : "r");
    if (!pipe) {
        return std::string();
    }

    std::string result;
    if (stdinData) {
        fwrite(stdinData->data(), 1, stdinData->size(), pipe);
    } else {
        std::array<char, 4096> buffer{};
        size_t bytesRead = 0;
        while ((bytesRead = fread(buffer.data(), 1, buffer.size(), pipe)) > 0) {
            result.append(buffer.data(), bytesRead);
        }
    }

    int status = pclose(pipe);
    *ok = (status == 0);
    return result;
}

static bool CopyLinux(const std::string& string) {
    bool ok = false;
    RunCommand("xclip -selection clipboard -in 2>/dev/null", &string, &ok);
    if (ok) {
        return true;
    }

    RunCommand("xsel --clipboard --input 2>/dev/null", &string, &ok);
    return ok;
}

static std::string GetLinux() {
    bool ok = false;
    std::string result = RunCommand("xclip -selection clipboard -out 2>/dev/null", nullptr, &ok);
    if (ok) {
        return result;
    }

    result = RunCommand("xsel --clipboard --output 2>/dev/null", nullptr, &ok);
    return ok ? result : std::string();
}

bool gl::Clipboard::CopyString(const std::string& string) {
    return CopyLinux(string);
}

std::string gl::Clipboard::GetString() {
    return GetLinux();
}

#endif
