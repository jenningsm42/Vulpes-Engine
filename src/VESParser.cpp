#define VULPESENGINE_EXPORT

#include <cstring>

#include <vulpes/VESParser.hpp>

#include "Logger.h"


namespace vul {
    bool VESParser::parse(Handle<Skeleton>& skeleton, const uint8_t* buffer, uint32_t size) {
        /* VULPES ENGINE SKELETON FORMAT SPECIFICATION
            Header:
                magic(4): int8_t[4] "VULS"
                version(2): uint16_t 0x0001
                bone count(1): uint8_t
                action count(1): uint8_t
                frames per second(4): float
            Data:
                Bone Contents[bone count]:
                    bone index(1): uint8_t
                    bone name length(1): uint8_t
                    bone name(bone name length): int8_t[]
                    bone head position(4): float
                    bone tail position(4): float
                Actions[action count]:
                    action name length(1): uint8_t
                    action name(action name length): int8_t[]
                    frame count(2): uint16_t
                    Bone States[frame count]:
                        locations(bone count): {float, float, float}
                        quaternions(bone count): {float, float, float, float}
        */

        if (memcmp(buffer, "VULS", 4) != 0) {
            Logger::log("vul::VESParser::parse: Invalid skeleton format");
            return false;
        }

        uint32_t curpos = 4;
        uint16_t version = *(uint16_t*)&buffer[curpos];
        curpos += 2;
        if (version != 1) {
            Logger::log("vul::VESParser::parse: Invalid version number");
            return false;
        }

        uint8_t boneCount = buffer[curpos++];
        if (boneCount == 0) {
            Logger::log("vul::VESParser::parse: No bones found");
            return false;
        }

        uint8_t actionCount = buffer[curpos++];
        if (actionCount == 0) {
            Logger::log("vul::VESParser::parse: No actions found");
            return false;
        }

        float fps = *(float*)&buffer[curpos];
        curpos += 4;

        skeleton->setFrameRate(fps);

        BoneIndexToDetailsMap bones;
        for (uint8_t i = 0; i < boneCount; i++) {
            size_t boneIndex = static_cast<size_t>(buffer[curpos++]);
            uint8_t nameLength = buffer[curpos++];
            std::string name;
            name.assign(&buffer[curpos], &buffer[curpos + nameLength]);
            curpos += nameLength;

            auto headAndTailBuffer = (const float*)(buffer + curpos);
            glm::vec3 head = glm::vec3(
                headAndTailBuffer[0],
                headAndTailBuffer[1],
                headAndTailBuffer[2]);
            glm::vec3 tail = glm::vec3(
                headAndTailBuffer[3],
                headAndTailBuffer[4],
                headAndTailBuffer[5]);
            curpos += 6 * sizeof(float);

            bones.emplace(boneIndex, std::make_tuple(name, head, tail));
        }

        skeleton->setBoneMap(std::move(bones));

        for (uint8_t i = 0; i < actionCount; i++) {
            uint8_t nameLength = buffer[curpos++];
            std::string name;
            name.assign(&buffer[curpos], &buffer[curpos + nameLength]);
            curpos += nameLength;

            Action action;
            uint16_t frameCount = *(uint16_t*)&buffer[curpos];
            curpos += 2;
            for (uint16_t j = 0; j < frameCount; j++) {
                FrameState frameState;

                auto locationsBuffer = (const float*)(buffer + curpos);
                curpos += 3 * boneCount * sizeof(float);
                auto quaternionsBuffer = (const float*)(buffer + curpos);
                curpos += 4 * boneCount * sizeof(float);

                for (uint8_t k = 0; k < boneCount; k++) {
                    BoneState boneState;
                    boneState.first = glm::vec3(
                        locationsBuffer[3 * k],
                        locationsBuffer[3 * k + 1],
                        locationsBuffer[3 * k + 2]);
                    boneState.second = glm::quat(
                        quaternionsBuffer[4 * k],
                        quaternionsBuffer[4 * k + 1],
                        quaternionsBuffer[4 * k + 2],
                        quaternionsBuffer[4 * k + 3]);

                    frameState.push_back(std::move(boneState));
                }

                action.push_back(std::move(frameState));
            }

            skeleton->addAction(name, std::move(action));
        }

        return true;
    }
}