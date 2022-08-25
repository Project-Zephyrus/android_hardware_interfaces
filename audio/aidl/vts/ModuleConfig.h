/*
 * Copyright (C) 2022 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <optional>
#include <set>
#include <utility>
#include <vector>

#include <aidl/android/hardware/audio/core/AudioRoute.h>
#include <aidl/android/hardware/audio/core/IModule.h>
#include <aidl/android/media/audio/common/AudioPort.h>

class ModuleConfig {
  public:
    using SrcSinkPair = std::pair<aidl::android::media::audio::common::AudioPortConfig,
                                  aidl::android::media::audio::common::AudioPortConfig>;
    using SrcSinkGroup =
            std::pair<aidl::android::hardware::audio::core::AudioRoute, std::vector<SrcSinkPair>>;

    explicit ModuleConfig(aidl::android::hardware::audio::core::IModule* module);
    const ndk::ScopedAStatus& getStatus() const { return mStatus; }
    std::string getError() const { return mStatus.getMessage(); }

    std::vector<aidl::android::media::audio::common::AudioPort> getAttachedDevicePorts() const;
    std::vector<aidl::android::media::audio::common::AudioPort> getExternalDevicePorts() const;
    std::vector<aidl::android::media::audio::common::AudioPort> getInputMixPorts() const;
    std::vector<aidl::android::media::audio::common::AudioPort> getOutputMixPorts() const;
    std::vector<aidl::android::media::audio::common::AudioPort> getMixPorts(bool isInput) const {
        return isInput ? getInputMixPorts() : getOutputMixPorts();
    }

    std::vector<aidl::android::media::audio::common::AudioPort> getAttachedDevicesPortsForMixPort(
            bool isInput, const aidl::android::media::audio::common::AudioPort& mixPort) const {
        return isInput ? getAttachedSourceDevicesPortsForMixPort(mixPort)
                       : getAttachedSinkDevicesPortsForMixPort(mixPort);
    }
    std::vector<aidl::android::media::audio::common::AudioPort>
    getAttachedSinkDevicesPortsForMixPort(
            const aidl::android::media::audio::common::AudioPort& mixPort) const;
    std::vector<aidl::android::media::audio::common::AudioPort>
    getAttachedSourceDevicesPortsForMixPort(
            const aidl::android::media::audio::common::AudioPort& mixPort) const;
    std::optional<aidl::android::media::audio::common::AudioPort>
    getSourceMixPortForAttachedDevice() const;

    std::optional<SrcSinkPair> getNonRoutableSrcSinkPair(bool isInput) const;
    std::optional<SrcSinkPair> getRoutableSrcSinkPair(bool isInput) const;
    std::vector<SrcSinkGroup> getRoutableSrcSinkGroups(bool isInput) const;

    std::vector<aidl::android::media::audio::common::AudioPortConfig>
    getPortConfigsForAttachedDevicePorts() const {
        return generateAudioDevicePortConfigs(getAttachedDevicePorts(), false);
    }
    std::vector<aidl::android::media::audio::common::AudioPortConfig> getPortConfigsForMixPorts()
            const {
        auto inputs = generateInputAudioMixPortConfigs(getInputMixPorts(), false);
        auto outputs = generateOutputAudioMixPortConfigs(getOutputMixPorts(), false);
        inputs.insert(inputs.end(), outputs.begin(), outputs.end());
        return inputs;
    }
    std::vector<aidl::android::media::audio::common::AudioPortConfig> getPortConfigsForMixPorts(
            bool isInput) const {
        return isInput ? generateInputAudioMixPortConfigs(getInputMixPorts(), false)
                       : generateOutputAudioMixPortConfigs(getOutputMixPorts(), false);
    }
    std::vector<aidl::android::media::audio::common::AudioPortConfig> getPortConfigsForMixPorts(
            bool isInput, const aidl::android::media::audio::common::AudioPort& port) const {
        return isInput ? generateInputAudioMixPortConfigs({port}, false)
                       : generateOutputAudioMixPortConfigs({port}, false);
    }
    std::optional<aidl::android::media::audio::common::AudioPortConfig> getSingleConfigForMixPort(
            bool isInput) const {
        const auto config = isInput ? generateInputAudioMixPortConfigs(getInputMixPorts(), true)
                                    : generateOutputAudioMixPortConfigs(getOutputMixPorts(), true);
        // TODO: Avoid returning configs for offload since they require an extra
        //       argument to openOutputStream.
        if (!config.empty()) {
            return *config.begin();
        } else {
            return {};
        }
    }
    std::optional<aidl::android::media::audio::common::AudioPortConfig> getSingleConfigForMixPort(
            bool isInput, const aidl::android::media::audio::common::AudioPort& port) const {
        const auto config = isInput ? generateInputAudioMixPortConfigs({port}, true)
                                    : generateOutputAudioMixPortConfigs({port}, true);
        if (!config.empty()) {
            return *config.begin();
        } else {
            return {};
        }
    }

    std::vector<aidl::android::media::audio::common::AudioPortConfig> getPortConfigsForDevicePort(
            const aidl::android::media::audio::common::AudioPort& port) const {
        return generateAudioDevicePortConfigs({port}, false);
    }
    aidl::android::media::audio::common::AudioPortConfig getSingleConfigForDevicePort(
            const aidl::android::media::audio::common::AudioPort& port) const {
        const auto config = generateAudioDevicePortConfigs({port}, true);
        return *config.begin();
    }

    std::string toString() const;

  private:
    std::vector<aidl::android::media::audio::common::AudioPortConfig>
    generateInputAudioMixPortConfigs(
            const std::vector<aidl::android::media::audio::common::AudioPort>& ports,
            bool singleProfile) const;
    std::vector<aidl::android::media::audio::common::AudioPortConfig>
    generateOutputAudioMixPortConfigs(
            const std::vector<aidl::android::media::audio::common::AudioPort>& ports,
            bool singleProfile) const;

    // Unlike MixPorts, the generator for DevicePorts always returns a non-empty
    // vector for a non-empty input port list. If there are no profiles in the
    // port, its initial configs are looked up, if there are none,
    // then an empty config is used, assuming further negotiation via setAudioPortConfig.
    std::vector<aidl::android::media::audio::common::AudioPortConfig>
    generateAudioDevicePortConfigs(
            const std::vector<aidl::android::media::audio::common::AudioPort>& ports,
            bool singleProfile) const;

    ndk::ScopedAStatus mStatus = ndk::ScopedAStatus::ok();
    std::vector<aidl::android::media::audio::common::AudioPort> mPorts;
    std::vector<aidl::android::media::audio::common::AudioPortConfig> mInitialConfigs;
    std::set<int32_t> mAttachedSinkDevicePorts;
    std::set<int32_t> mAttachedSourceDevicePorts;
    std::set<int32_t> mExternalDevicePorts;
    std::vector<aidl::android::hardware::audio::core::AudioRoute> mRoutes;
};
