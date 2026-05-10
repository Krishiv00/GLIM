#include <vector>
#include <fstream>

#include "GLIM/Audio/Sound.hpp"

using namespace gl;

void Sound::reuploadBuffer() {
    switch (detectFormat(m_Path)) {
    case AudioFormat::WAV: loadWAV(); break;
    case AudioFormat::MP3: loadMP3(); break;
    default: throw std::runtime_error("Sound: unsupported format: \"" + m_Path + "\"");
    }
}

void Sound::loadWAV() {
    std::ifstream file(m_Path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Sound: cannot open file \"" + m_Path + "\"");
    }

    const AudioHeader hdr = parseWAVHeader(file, m_Path);
    const std::vector<char> pcm(std::istreambuf_iterator<char>(file), {});

    alBufferData(
        m_Buffer, hdr.AlFormat,
        pcm.data(), static_cast<ALsizei>(pcm.size()),
        hdr.SampleRate
    );

    alSourcei(m_Source, AL_BUFFER, static_cast<ALint>(m_Buffer));
}

void Sound::loadMP3() {
    const MP3PCM mp3 = decodeMP3(m_Path);

    alBufferData(
        m_Buffer, mp3.Header.AlFormat,
        mp3.Samples.data(),
        static_cast<ALsizei>(mp3.Samples.size() * sizeof(int16_t)),
        mp3.Header.SampleRate
    );
    
    alSourcei(m_Source, AL_BUFFER, static_cast<ALint>(m_Buffer));
}

void Sound::LoadFromFile(const std::string& path) {
    m_Path = path;
    reuploadBuffer();
    m_Loaded = true;
}

void Sound::Play() {
    if (m_Loaded) alSourcePlay(m_Source);
}

void Sound::onDeviceChanged(const SourceSnapshot& snapshot) {
    if (m_Source) {
        alSourceStop(m_Source);
        alDeleteSources(1, &m_Source);
        m_Source = 0;
    }

    if (m_Buffer) {
        alDeleteBuffers(1, &m_Buffer);
        m_Buffer = 0;
    }

    alGenSources(1, &m_Source);
    alGenBuffers(1, &m_Buffer);

    if (!m_Loaded) return;

    reuploadBuffer();
    applySnapshot(snapshot);
    alSourcef(m_Source, AL_SEC_OFFSET, snapshot.OffsetSeconds);

    if (snapshot.State == AL_PLAYING) {
        alSourcePlay(m_Source);
    } else if (snapshot.State == AL_PAUSED) {
        alSourcePlay(m_Source);
        alSourcePause(m_Source);
    }
}