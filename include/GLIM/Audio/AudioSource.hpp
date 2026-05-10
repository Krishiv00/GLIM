#pragma once

#include <string>

#include "AL/al.h"

#include "GLIM/Audio/AudioDevice.hpp"

namespace gl {
    class AudioSource {
        friend class AudioDevice;

    protected:
        enum class AudioFormat {
            WAV, MP3, Unknown
        };

        struct AudioHeader {
            ALenum AlFormat = AL_FORMAT_MONO16;
            int SampleRate = 44100;
            int Channels = 1;
            std::streampos DataOffset = 0;
        };

        struct MP3PCM {
            AudioHeader Header;
            std::vector<int16_t> Samples;
        };

        struct SourceSnapshot {
            float Gain = 1.f;
            float Pitch = 1.f;
            bool Looping = false;
            ALint State = AL_STOPPED;
            float OffsetSeconds = 0.f;
        };

        static AudioFormat detectFormat(const std::string& path);

        static AudioHeader parseWAVHeader(std::ifstream& file, const std::string& path);

        static MP3PCM decodeMP3(const std::string& path);

        struct NoAlloc {};
        explicit AudioSource(NoAlloc) {}

        SourceSnapshot captureSnapshot() const;
        void applySnapshot(const SourceSnapshot& s);

        ALint sourceState() const;

        ALuint m_Source = 0;

    public:
        AudioSource();
        virtual ~AudioSource();

        AudioSource(const AudioSource&) = delete;
        AudioSource& operator=(const AudioSource&) = delete;

        virtual void Play() = 0;
        virtual void Pause();
        virtual void Resume();
        virtual void Stop();

        void SetVolume(float v);
        void SetPitch(float p);

        void SetLoop(bool loop);

        [[nodiscard]]
        float GetVolume() const;

        [[nodiscard]]
        float GetPitch()  const;

        [[nodiscard]]
        bool IsPlaying() const;

        [[nodiscard]]
        bool IsPaused()  const;

        [[nodiscard]]
        bool IsStopped() const;

        virtual void onDeviceChanged(const SourceSnapshot& snapshot) = 0;
    };
}