#pragma once

#include <raylib.h>

#include "../BaseScene.hpp"
#include "scenes/Scenes.hpp"

class InstructionsScene : public BaseScene {
  private:
    Texture2D background_;
    Texture2D santa_walk_;
    Rectangle santa_frame_;

    Font title_font_;
    Font instructions_font_;
    static constexpr int kSupplementalFontSize = 48;

    static constexpr int kTitleFontSize = 96;
    Vector2 title_text_size_;
    Vector2 title_text_position_;
    static constexpr char kTitleText[] = "Instructions";

    Vector2 play_text_size_;
    Vector2 play_text_position_;
    static constexpr char kPlayText[] = "Play";

  public:
    InstructionsScene();
    ~InstructionsScene();

    void draw() override;
    SceneChangeRequest update(float dt) override;
};
