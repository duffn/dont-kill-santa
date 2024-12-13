#pragma once

#include "Scenes.hpp"

class BaseScene {
  public:
    virtual ~BaseScene() = default;
    virtual void draw() = 0;
    virtual SceneChangeRequest update(float dt) = 0;
};
