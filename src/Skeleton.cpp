#define VULPESENGINE_EXPORT

#include <vulpes/Skeleton.hpp>
#include <vulpes/ResourceLoader.hpp>
#include <vulpes/Mesh.hpp>

#include "Logger.h"


namespace vul {
    Skeleton::Skeleton() : m_fps(25.f), m_currentFrame(0.f) {
    }

    void Skeleton::update(float deltaTime) noexcept {
        if (m_actions.find(m_currentAction) == m_actions.end()) {
            return;
        }

        auto action = m_actions[m_currentAction];

        // Cycle each action
        m_currentFrame += m_fps * deltaTime;

        auto integerFrame = static_cast<size_t>(m_currentFrame);
        if (integerFrame >= action.size()) {
            m_currentFrame -= integerFrame;
        }
    }

    FrameState Skeleton::getCurrentFrameState() {
        return getCurrentFrameState(m_boneNameToIndex);
    }

    FrameState Skeleton::getCurrentFrameState(BoneNameToIndexMap boneNamesToIndex) {
        if (m_actions.find(m_currentAction) == m_actions.end()) {
            return FrameState();
        }

        auto action = m_actions[m_currentAction];

        if (action.size() == 1) {
            return action[0];
        }

        auto previousFrame = static_cast<size_t>(m_currentFrame);
        auto nextFrame = previousFrame + 1;
        if (nextFrame >= action.size()) {
            nextFrame = 0;
        }

        float alpha = m_currentFrame - previousFrame;

        auto& previousFrameState = action[previousFrame];
        auto& nextFrameState = action[nextFrame];

        FrameState frameState(previousFrameState.size());
        for (size_t i = 0; i < previousFrameState.size(); i++) {
            BoneState boneState;
            boneState.first = glm::mix(
                previousFrameState[i].first,
                nextFrameState[i].first,
                alpha);
            boneState.second = glm::slerp(
                previousFrameState[i].second,
                nextFrameState[i].second,
                alpha);

            size_t index = boneNamesToIndex[std::get<0>(m_boneIndexToDetails[i])];
            frameState[index] = boneState;
        }

        return frameState;
    }

    float Skeleton::getCurrentFrame() noexcept {
        return m_currentFrame;
    }

    BoneDetailMap Skeleton::getBoneDetailMap() {
        return getBoneDetailMap(m_boneNameToIndex);
    }

    BoneDetailMap Skeleton::getBoneDetailMap(BoneNameToIndexMap boneNamesToIndex) {
        BoneDetailMap boneDetailMap(m_boneIndexToDetails.size());
        for (auto& it : m_boneIndexToDetails) {
            auto index = boneNamesToIndex[std::get<0>(it.second)];
            boneDetailMap[index] = it.second;
        }

        return boneDetailMap;
    }

    void Skeleton::addAction(const std::string& name, Action&& action) {
        m_actions.emplace(name, std::move(action));

        if (m_currentAction == "") {
            m_currentAction = name;
        }
    }

    void Skeleton::setAction(const std::string& name) {
        if (m_actions.find(name) == m_actions.end()) {
            Logger::log("vul::Skeleton::setAction: Action '%s' not found", name);
            return;
        }

        m_currentAction = name;
    }

    void Skeleton::setBoneMap(BoneIndexToDetailsMap&& boneIndexToDetails) {
        m_boneIndexToDetails = std::move(boneIndexToDetails);

        m_boneNameToIndex.clear();
        for (auto& it : m_boneIndexToDetails) {
            m_boneNameToIndex[std::get<0>(it.second)] = it.first;
        }
    }

    void Skeleton::setFrameRate(float fps) {
        m_fps = fps;
    }
}