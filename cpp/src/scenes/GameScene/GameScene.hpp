#pragma once

#include <raylib.h>

#include "../BaseScene.hpp"
#include "scenes/Scenes.hpp"

class GameScene : public BaseScene {
  private:
    static constexpr int kMaxFrameSpeed = 45;
    static constexpr float kFrameMultiplier = 40.0f;
    static constexpr float kAcceleration = 200.0f;
    static constexpr float kDeceleration = 100.0f;

    Texture2D background_;
    Texture2D ground_;
    Texture2D santa_dead_;
    Texture2D santa_walk_;
    Texture2D candy_cane_;

    Texture2D *current_santa_;
    Rectangle santa_frame_;
    Vector2 santa_position_;

    Vector2 candy_cane_position_;

    Font font_;
    Font menu_font_;
    static constexpr int kFontSize = 36;

    static constexpr char kRestartText[] = "Play Again";
    Vector2 restart_text_size_;
    Vector2 restart_text_position_;

    static constexpr char kQuitText[] = "Quit";
    Vector2 quit_text_size_;
    Vector2 quit_text_position_;

    static constexpr char kGameOverText[] = "You killed Santa";
    Vector2 game_over_text_size_;
    Vector2 game_over_text_position_;

    bool game_started_ = false;
    bool game_ended_ = false;
    bool is_dead_ = false;
    bool is_animating_death_ = false;
    float current_difference_ = 0.0f;
    float final_difference_ = 0.0f;
    int ground_y_ = 0;

    float frames_speed_ = 0.0f;
    int frames_counter_ = 0;
    int current_frame_ = 0;
    float scrolling_back_ = 0.0f;

  public:
    GameScene();
    ~GameScene();

    void draw() override;
    SceneChangeRequest update(float dt) override;
};
