#ifndef _VUL_SKELETON_HPP
#define _VUL_SKELETON_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Export.hpp"


namespace vul {
    using BoneState = std::pair<glm::vec3, glm::quat>;
    using BoneDetails = std::tuple<std::string, glm::vec3, glm::vec3>; // Name, head, tail
    using FrameState = std::vector<BoneState>; // Bone index -> bone state
    using BoneDetailMap = std::vector<BoneDetails>; // Bone index -> bone details
    using Action = std::vector<FrameState>;
    using ActionMap = std::unordered_map<std::string, Action>;
    using BoneNameToIndexMap = std::unordered_map<std::string, size_t>;
    using BoneIndexToDetailsMap = std::unordered_map<size_t, BoneDetails>;

    class VEAPI Skeleton {
    public:
        Skeleton();

        void update(float deltaTime) noexcept;

        FrameState getCurrentFrameState();
        FrameState getCurrentFrameState(BoneNameToIndexMap boneNamesToIndex);
        float getCurrentFrame() noexcept;

        BoneDetailMap getBoneDetailMap();
        BoneDetailMap getBoneDetailMap(BoneNameToIndexMap boneNameToIndex);

        void addAction(const std::string&, Action&&);
        void setAction(const std::string&);

        void setBoneMap(BoneIndexToDetailsMap&&);
        void setFrameRate(float fps);

    private:
        float m_fps;
        float m_currentFrame;
        BoneNameToIndexMap m_boneNameToIndex;
        BoneIndexToDetailsMap m_boneIndexToDetails;
        ActionMap m_actions;
        std::string m_currentAction;
    };
}

#endif // _VUL_SKELETON_HPP
