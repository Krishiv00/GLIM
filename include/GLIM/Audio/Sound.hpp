#pragma once

#include "GLIM/Audio/AudioSource.hpp"

namespace gl {
    class Sound : public AudioSource {
    private:
        void reuploadBuffer();
        void loadWAV();
        void loadMP3();

        std::string m_Path;
        ALuint m_Buffer = 0;
        bool m_Loaded = false;

    public:
        Sound() {
            alGenBuffers(1, &m_Buffer);
        }

        explicit Sound(const std::string& path) : Sound() {
            LoadFromFile(path);
        }

        ~Sound() {
            if (m_Buffer) alDeleteBuffers(1, &m_Buffer);
        }

        Sound(Sound&& o) noexcept : AudioSource(NoAlloc{}) {
            m_Source = o.m_Source;
            m_Buffer = o.m_Buffer;
            m_Loaded = o.m_Loaded;
            m_Path = std::move(o.m_Path);

            o.m_Source = 0;
            o.m_Buffer = 0;
            o.m_Loaded = false;

            if (m_Source) {
                AudioDevice::instance().unregisterSource(&o);
                AudioDevice::instance().registerSource(this);
            }
        }

        Sound& operator=(Sound&& o) noexcept {
            if (this == &o) return *this;

            if (m_Source) {
                alSourceStop(m_Source);
                alDeleteSources(1, &m_Source);
                AudioDevice::instance().unregisterSource(this);
            }
            if (m_Buffer) alDeleteBuffers(1, &m_Buffer);

            m_Source = o.m_Source;
            m_Buffer = o.m_Buffer;
            m_Loaded = o.m_Loaded;
            m_Path = std::move(o.m_Path);

            o.m_Source = 0;
            o.m_Buffer = 0;
            o.m_Loaded = false;

            if (m_Source) {
                AudioDevice::instance().unregisterSource(&o);
                AudioDevice::instance().registerSource(this);
            }

            return *this;
        }

        void LoadFromFile(const std::string& path);

        void Play() override;

        void onDeviceChanged(const SourceSnapshot& snapshot) override;
    };
}