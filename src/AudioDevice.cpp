#include <vector>

#include "GLIM/Audio/AudioDevice.hpp"
#include "GLIM/Audio/AudioSource.hpp"

using namespace gl;

std::string AudioDevice::defaultDeviceName() {
    const char* devices = alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);
    if (devices && *devices) return std::string(devices);

    const char* def = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
    return def ? std::string(def) : std::string();
}

void AudioDevice::openDevice(const std::string& name) {
    m_Device = alcOpenDevice(name.empty() ? nullptr : name.c_str());
    if (!m_Device) {
        throw std::runtime_error("AudioDevice: failed to open device: " + name);
    }

    m_Context = alcCreateContext(m_Device, nullptr);
    if (!m_Context) {
        alcCloseDevice(m_Device);
        throw std::runtime_error("AudioDevice: failed to create AL context");
    }

    alcMakeContextCurrent(m_Context);
}

void AudioDevice::closeDevice() {
    alcMakeContextCurrent(nullptr);

    if (m_Context) {
        alcDestroyContext(m_Context);
        m_Context = nullptr;
    }

    if (m_Device) {
        alcCloseDevice(m_Device);
        m_Device = nullptr;
    }
}

void AudioDevice::checkDeviceChange() {
    const std::string newName = defaultDeviceName();
    if (newName == m_CurrentDeviceName) return;

    std::lock_guard<std::mutex> lk(m_SourcesMutex);

    std::vector<std::pair<AudioSource*, AudioSource::SourceSnapshot>> snapshots;

    snapshots.reserve(m_Sources.size());

    for (AudioSource* src : m_Sources) {
        snapshots.push_back({src, src->captureSnapshot()});
    }

    closeDevice();

    m_CurrentDeviceName = newName;

    try {
        openDevice(m_CurrentDeviceName);
    }
    catch (...) {
        m_CurrentDeviceName.clear();
        openDevice(m_CurrentDeviceName);
    }

    for (auto& [src, snap] : snapshots) {
        src->onDeviceChanged(snap);
    }
}

AudioDevice::AudioDevice() {
    if (s_Instance) {
        throw std::runtime_error("AudioDevice: only one instance allowed at a time.");
    }

    s_Instance = this;

    m_CurrentDeviceName = defaultDeviceName();
    openDevice(m_CurrentDeviceName);

    m_PollThread = std::thread([this] {
        while (!m_StopPoll.load()) {
            std::this_thread::sleep_for(std::chrono::seconds(DeviceChangeCheckInterval));
            
            if (!m_StopPoll.load()) checkDeviceChange();
        }
    });
}

AudioDevice::~AudioDevice() {
    m_StopPoll.store(true);
    if (m_PollThread.joinable()) m_PollThread.join();

    closeDevice();
    s_Instance = nullptr;
}

AudioDevice& AudioDevice::instance() {
    if (!s_Instance) {
        throw std::runtime_error("AudioDevice: no instance exists. Create one first.");
    }

    return *s_Instance;
}

void AudioDevice::registerSource(AudioSource* src) {
    std::lock_guard<std::mutex> lk(m_SourcesMutex);
    m_Sources.insert(src);
}

void AudioDevice::unregisterSource(AudioSource* src) {
    std::lock_guard<std::mutex> lk(m_SourcesMutex);
    m_Sources.erase(src);
}