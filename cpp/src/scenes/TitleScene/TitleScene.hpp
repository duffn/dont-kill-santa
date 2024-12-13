#pragma once

#include <raylib.h>

#include "../BaseScene.hpp"
#include "scenes/Scenes.hpp"

class TitleScene : public BaseScene {
  private:
    Texture2D background_;
    Texture2D santa_dead_;
    Rectangle santa_frame_;

    Font title_font_;
    Font menu_font_;

    static constexpr int kTitleFontSize = 96;
    Vector2 title_text_size_;
    Vector2 title_text_position_;
    static constexpr char kTitleText[] = "Don't Kill Santa";

    static constexpr int kSupplementalFontSize = 48;

    Vector2 start_text_size_;
    Vector2 start_text_position_;
    static constexpr char kStartText[] = "Start";

    Vector2 instructions_text_size_;
    Vector2 instructions_text_position_;
    static constexpr char kInstructionsText[] = "Instructions";

  public:
    TitleScene();
    ~TitleScene();

    void draw() override;
    SceneChangeRequest update(float dt) override;
};
