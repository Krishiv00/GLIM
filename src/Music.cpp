#include "GLIM/Audio/Music.hpp"

using namespace gl;

void Music::rewindStream() {
    if (m_Format == AudioFormat::WAV) {
        m_File.clear();
        m_File.seekg(m_Header.DataOffset);
    } else {
        mp3dec_ex_seek(&m_Mp3stream, 0);
    }
}

void Music::seekToSeconds(float seconds) {
    if (seconds <= 0.f) {
        rewindStream();
        return;
    }

    if (m_Format == AudioFormat::WAV) {
        const int bytesPerSec = m_Header.SampleRate * m_Header.Channels * 2;

        const std::streamoff offset = static_cast<std::streamoff>(
            seconds * static_cast<float>(bytesPerSec)
        );

        m_File.clear();
        m_File.seekg(m_Header.DataOffset + offset);
    } else {
        const uint64_t sample = static_cast<uint64_t>(
            seconds * static_cast<float>(m_Header.SampleRate)
        );
        
        mp3dec_ex_seek(&m_Mp3stream, sample);
    }
}

void Music::startStream() {
    m_StopFlag.store(false);
    m_PauseFlag.store(false);

    for (int i = 0; i < NUM_BUFFERS; ++i) {
        fillBuffer(m_Buffers[i]);
    }

    alSourceQueueBuffers(m_Source, NUM_BUFFERS, m_Buffers);
    alSourcePlay(m_Source);

    m_StreamThread = std::thread(&Music::streamLoop, this);
}

bool Music::fillBuffer(ALuint buffer) {
    if (m_Format == AudioFormat::WAV) {
        m_Chunk.resize(BUFFER_SIZE);
        m_File.read(m_Chunk.data(), static_cast<std::streamsize>(BUFFER_SIZE));

        const std::streamsize n = m_File.gcount();
        if (n <= 0) return false;

        alBufferData(
            buffer, m_Header.AlFormat,
            m_Chunk.data(), static_cast<ALsizei>(n),
            m_Header.SampleRate
        );
    } else {
        const std::size_t samplesWanted = BUFFER_SIZE / sizeof(mp3d_sample_t);
        m_Mp3Chunk.resize(samplesWanted);

        const std::size_t samplesRead = mp3dec_ex_read(
            &m_Mp3stream, m_Mp3Chunk.data(), samplesWanted
        );

        if (samplesRead == 0) return false;

        alBufferData(
            buffer, m_Header.AlFormat, m_Mp3Chunk.data(),
            static_cast<ALsizei>(samplesRead * sizeof(mp3d_sample_t)), m_Header.SampleRate
        );
    }

    return true;
}

void Music::streamLoop() {
    while (!m_StopFlag.load()) {
        if (m_PauseFlag.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        ALint processed = 0;
        alGetSourcei(m_Source, AL_BUFFERS_PROCESSED, &processed);

        while (processed-- > 0) {
            ALuint buf;
            alSourceUnqueueBuffers(m_Source, 1, &buf);

            if (!fillBuffer(buf)) {
                if (m_Loop) {
                    rewindStream();
                    fillBuffer(buf);
                } else {
                    alSourceQueueBuffers(m_Source, 1, &buf);
                    m_StopFlag.store(true);
                    return;
                }
            } else {
                alSourceQueueBuffers(m_Source, 1, &buf);
            }
        }

        if (!m_StopFlag.load() && !m_PauseFlag.load() && !IsPlaying()) {
            ALint queued = 0;
            alGetSourcei(m_Source, AL_BUFFERS_QUEUED, &queued);

            if (queued > 0) alSourcePlay(m_Source);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void Music::LoadFromFile(const std::string& path) {
    Stop();

    if (m_Mp3Opened) {
        mp3dec_ex_close(&m_Mp3stream);
        m_Mp3Opened = false;
    }

    m_Path = path;
    m_Format = detectFormat(path);

    if (m_Format == AudioFormat::WAV) {
        m_File.open(path, std::ios::binary);

        if (!m_File) {
            throw std::runtime_error("Music: cannot open file \"" + path + "\"");
        }

        m_Header = parseWAVHeader(m_File, path);
    } else if (m_Format == AudioFormat::MP3) {
        mp3dec_init(&m_Mp3dec);

        if (mp3dec_ex_open(&m_Mp3stream, path.c_str(), MP3D_SEEK_TO_SAMPLE) != 0) {
            throw std::runtime_error("Music: failed to open MP3: \"" + path + "\"");
        }

        m_Mp3Opened = true;

        m_Header.SampleRate = m_Mp3stream.info.hz;
        m_Header.Channels = m_Mp3stream.info.channels;
        m_Header.AlFormat = m_Header.Channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    } else {
        throw std::runtime_error("Music: unsupported format: \"" + path + "\"");
    }

    if (!m_BuffersGenerated) {
        alGenBuffers(NUM_BUFFERS, m_Buffers);
        m_BuffersGenerated = true;
    }

    m_Loaded = true;
}

void Music::Play() {
    if (!m_Loaded) return;

    Stop();
    rewindStream();
    startStream();
}

void Music::Pause() {
    if (!IsPlaying()) return;

    m_PauseFlag.store(true);
    alSourcePause(m_Source);
}

void Music::Resume() {
    if (!IsPaused()) return;

    m_PauseFlag.store(false);
    alSourcePlay(m_Source);
}

void Music::Stop() {
    m_StopFlag.store(true);
    if (m_StreamThread.joinable()) m_StreamThread.join();

    if (m_Source) {
        alSourceStop(m_Source);

        ALint queued = 0;
        alGetSourcei(m_Source, AL_BUFFERS_QUEUED, &queued);

        while (queued-- > 0) {
            ALuint buf;
            alSourceUnqueueBuffers(m_Source, 1, &buf);
        }
    }
}

void Music::SetLoop(bool loop) {
    m_Loop = loop;
}

void Music::onDeviceChanged(const SourceSnapshot& snapshot) {
    m_StopFlag.store(true);
    if (m_StreamThread.joinable()) m_StreamThread.join();

    if (m_Source) {
        alSourceStop(m_Source);

        ALint queued = 0;
        alGetSourcei(m_Source, AL_BUFFERS_QUEUED, &queued);

        while (queued-- > 0) {
            ALuint buf;
            alSourceUnqueueBuffers(m_Source, 1, &buf);
        }

        alDeleteSources(1, &m_Source);
        m_Source = 0;
    }

    if (m_BuffersGenerated) {
        alDeleteBuffers(NUM_BUFFERS, m_Buffers);
        m_BuffersGenerated = false;
    }

    alGenSources(1, &m_Source);
    alGenBuffers(NUM_BUFFERS, m_Buffers);
    m_BuffersGenerated = true;

    if (!m_Loaded) return;

    applySnapshot(snapshot);
    seekToSeconds(snapshot.OffsetSeconds);

    if (snapshot.State == AL_PLAYING) {
        startStream();
    } else if (snapshot.State == AL_PAUSED) {
        startStream();
        std::this_thread::sleep_for(std::chrono::milliseconds(60));
        m_PauseFlag.store(true);
        alSourcePause(m_Source);
    }
}