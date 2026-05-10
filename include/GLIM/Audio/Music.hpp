#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <atomic>

#include "MMP3/minimp3_ex.h"

#include "GLIM/Audio/AudioSource.hpp"

namespace gl {
    class Music : public AudioSource {
    public:
        static constexpr int NUM_BUFFERS = 2;
        static constexpr std::size_t BUFFER_SIZE = 65536;

    private:
        void rewindStream();
        void seekToSeconds(float seconds);
        void startStream();
        bool fillBuffer(ALuint buffer);
        void streamLoop();

        std::string m_Path;
        AudioFormat m_Format = AudioFormat::Unknown;
        AudioHeader m_Header;

        std::ifstream m_File;

        mp3dec_t m_Mp3dec = {};
        mp3dec_ex_t m_Mp3stream = {};
        bool m_Mp3Opened = false;
        std::vector<mp3d_sample_t> m_Mp3Chunk;

        ALuint m_Buffers[NUM_BUFFERS] = {};
        bool m_BuffersGenerated = false;
        bool m_Loaded = false;
        bool m_Loop = false;

        std::vector<char> m_Chunk;
        std::thread m_StreamThread;
        std::atomic<bool> m_StopFlag{true};
        std::atomic<bool> m_PauseFlag{false};

    public:
        Music() : AudioSource() {}

        explicit Music(const std::string& path) : AudioSource() {
            LoadFromFile(path);
        }

        ~Music() {
            Stop();
            if (m_Mp3Opened) mp3dec_ex_close(&m_Mp3stream);
            if (m_BuffersGenerated) alDeleteBuffers(NUM_BUFFERS, m_Buffers);
        }

        Music(const Music&) = delete;
        Music& operator=(const Music&) = delete;

        void LoadFromFile(const std::string& path);

        void Play() override;
        void Pause() override;
        void Resume() override;
        void Stop() override;

        void SetLoop(bool loop);

        void onDeviceChanged(const SourceSnapshot& snapshot) override;
    };
}