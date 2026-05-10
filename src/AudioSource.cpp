#include <stdexcept>
#include <cstring>
#include <fstream>
#include <vector>

#include "MMP3/minimp3_ex.h"

#include "GLIM/Audio/AudioSource.hpp"

using namespace gl;

AudioSource::AudioFormat AudioSource::detectFormat(const std::string& path) {
    std::ifstream f(path, std::ios::binary);

    char magic[4]{};
    f.read(magic, 4);

    if (std::strncmp(magic, "RIFF", 4) == 0) return AudioFormat::WAV;
    if (std::strncmp(magic, "ID3", 3) == 0) return AudioFormat::MP3;

    if (
        (static_cast<unsigned char>(magic[0]) == 0xFF) &&
        ((static_cast<unsigned char>(magic[1]) & 0xE0) == 0xE0)
    ) return AudioFormat::MP3;

    return AudioFormat::Unknown;
}

AudioSource::AudioHeader AudioSource::parseWAVHeader(std::ifstream& file, const std::string& path) {
    const auto read16 = [&]() -> int16_t {
        int16_t v = 0;
        file.read(reinterpret_cast<char*>(&v), 2);
        return v;
        };

    const auto read32 = [&]() -> int32_t {
        int32_t v = 0;
        file.read(reinterpret_cast<char*>(&v), 4);
        return v;
        };

    char tag[4];

    file.read(tag, 4);
    if (std::strncmp(tag, "RIFF", 4) != 0) {
        throw std::runtime_error("Audio: not a RIFF file: \"" + path + "\"");
    }

    file.ignore(4); // RIFF chunk size

    file.read(tag, 4);
    if (std::strncmp(tag, "WAVE", 4) != 0) {
        throw std::runtime_error("Audio: not a WAVE file: \"" + path + "\"");
    }

    int16_t channels = 0;
    int16_t bitsPerSample = 0;

    AudioHeader out;

    while (file.read(tag, 4)) {
        int32_t chunkSize = read32();

        if (std::strncmp(tag, "fmt ", 4) == 0) {
            read16();
            channels = read16();
            out.SampleRate = read32();
            file.ignore(6);
            bitsPerSample = read16();
            file.ignore(chunkSize - 16);
        } else if (std::strncmp(tag, "data", 4) == 0) {
            out.DataOffset = file.tellg();
            break;
        } else {
            file.ignore(chunkSize);
        }
    }

    if (channels == 0) {
        throw std::runtime_error("Audio: missing fmt chunk in \"" + path + "\"");
    }

    out.Channels = channels;

    if (channels == 1 && bitsPerSample == 8)  out.AlFormat = AL_FORMAT_MONO8;
    else if (channels == 1 && bitsPerSample == 16) out.AlFormat = AL_FORMAT_MONO16;
    else if (channels == 2 && bitsPerSample == 8)  out.AlFormat = AL_FORMAT_STEREO8;
    else if (channels == 2 && bitsPerSample == 16) out.AlFormat = AL_FORMAT_STEREO16;
    else throw std::runtime_error("Audio: unsupported WAV channel/bit config");

    return out;
}

AudioSource::MP3PCM AudioSource::decodeMP3(const std::string& path) {
    mp3dec_t dec = {};
    mp3dec_file_info_t info = {};
    mp3dec_init(&dec);

    if (mp3dec_load(&dec, path.c_str(), &info, nullptr, nullptr) != 0) {
        throw std::runtime_error("Audio: failed to decode MP3: \"" + path + "\"");
    }

    MP3PCM out;
    out.Header.SampleRate = info.hz;
    out.Header.Channels = info.channels;
    out.Header.AlFormat = info.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    out.Samples.assign(info.buffer, info.buffer + info.samples);

    free(info.buffer);

    return out;
}

AudioSource::SourceSnapshot AudioSource::captureSnapshot() const {
    SourceSnapshot s;
    alGetSourcef(m_Source, AL_GAIN, &s.Gain);
    alGetSourcef(m_Source, AL_PITCH, &s.Pitch);
    alGetSourcef(m_Source, AL_SEC_OFFSET, &s.OffsetSeconds);

    ALint loop = 0;
    alGetSourcei(m_Source, AL_LOOPING, &loop);

    s.Looping = loop == AL_TRUE;
    s.State = sourceState();

    return s;
}

void AudioSource::applySnapshot(const SourceSnapshot& s) {
    alSourcef(m_Source, AL_GAIN, s.Gain);
    alSourcef(m_Source, AL_PITCH, s.Pitch);
    alSourcei(m_Source, AL_LOOPING, s.Looping ? AL_TRUE : AL_FALSE);
}

ALint AudioSource::sourceState() const {
    if (!m_Source) return AL_INITIAL;

    ALint s;
    alGetSourcei(m_Source, AL_SOURCE_STATE, &s);

    return s;
}

AudioSource::AudioSource() {
    alGenSources(1, &m_Source);
    AudioDevice::instance().registerSource(this);
}

AudioSource::~AudioSource() {
    if (AudioDevice::s_Instance) {
        AudioDevice::instance().unregisterSource(this);
    }

    if (m_Source) {
        alSourceStop(m_Source);
        alDeleteSources(1, &m_Source);
    }
}

void AudioSource::Pause() {
    alSourcePause(m_Source);
}

void AudioSource::Resume() {
    if (IsPaused()) alSourcePlay(m_Source);
}

void AudioSource::Stop() {
    if (m_Source) alSourceStop(m_Source);
}

void AudioSource::SetVolume(float v) {
    alSourcef(m_Source, AL_GAIN, v);
}

void AudioSource::SetPitch(float p) {
    alSourcef(m_Source, AL_PITCH, p);
}

void AudioSource::SetLoop(bool loop) {
    alSourcei(m_Source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

float AudioSource::GetVolume() const {
    float v = 1.f;
    alGetSourcef(m_Source, AL_GAIN, &v);
    return v;
}

float AudioSource::GetPitch() const {
    float p = 1.f;
    alGetSourcef(m_Source, AL_PITCH, &p);
    return p;
}

bool AudioSource::IsPlaying() const {
    return sourceState() == AL_PLAYING;
}

bool AudioSource::IsPaused()  const {
    return sourceState() == AL_PAUSED;
}

bool AudioSource::IsStopped() const {
    const ALint s = sourceState();
    return s == AL_STOPPED || s == AL_INITIAL;
}