#pragma once

#include <raylib.h>
#include <string>

namespace Constants {
const std::string kWindowTitle = "Don't Kill Santa";
const int kTargetFPS = 60;

const int kScreenWidth = 1280;
const int kScreenHeight = 720;

const int kSantaDeadNumFrames = 17;
const int kSantaWalkNumFrames = 13;

const Color kBackgroundColor = {47, 79, 146, 255};

inline std::string GetAssetPath(std::string assetName) {
    return ASSETS_PATH "" + assetName;
}
} // namespace Constants
