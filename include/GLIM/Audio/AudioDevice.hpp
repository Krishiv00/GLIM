#pragma once

#include <string>
#include <mutex>
#include <set>
#include <thread>
#include <atomic>

#include "AL/alc.h"

namespace gl {
    class AudioSource;

    class AudioDevice {
        friend class AudioSource;

    private:
        static std::string defaultDeviceName();
        void openDevice(const std::string& name);
        void closeDevice();
        void checkDeviceChange();

        static AudioDevice* s_Instance;

        ALCdevice* m_Device = nullptr;
        ALCcontext* m_Context = nullptr;

        std::string m_CurrentDeviceName;
        std::set<AudioSource*> m_Sources;
        std::mutex m_SourcesMutex;
        std::thread m_PollThread;
        std::atomic<bool> m_StopPoll{false};

    public:
        static constexpr int DeviceChangeCheckInterval = 1;

        AudioDevice();
        ~AudioDevice();

        AudioDevice(const AudioDevice&) = delete;
        AudioDevice& operator=(const AudioDevice&) = delete;

        static AudioDevice& instance();

        void registerSource(AudioSource* src);
        void unregisterSource(AudioSource* src);
    };

    inline AudioDevice* AudioDevice::s_Instance = nullptr;
}