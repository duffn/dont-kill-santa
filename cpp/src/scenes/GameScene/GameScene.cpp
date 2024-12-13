#include <raylib.h>

#include "../../Constants.hpp"
#include "../Scenes.hpp"
#include "GameScene.hpp"

GameScene::GameScene() {
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    background_ = LoadTexture(
        Constants::GetAssetPath("backgrounds/background.png").c_str());
    ground_ =
        LoadTexture(Constants::GetAssetPath("backgrounds/ground.png").c_str());
    font_ = LoadFontEx(Constants::GetAssetPath("fonts/hello-santa.ttf").c_str(),
                       96, 0, 0);
    menu_font_ = LoadFontEx(
        Constants::GetAssetPath("fonts/coolvetica.ttf").c_str(), 96, 0, 0);
    santa_dead_ =
        LoadTexture(Constants::GetAssetPath("sprites/santa-dead.png").c_str());
    santa_walk_ =
        LoadTexture(Constants::GetAssetPath("sprites/santa-walk.png").c_str());
    candy_cane_ =
        LoadTexture(Constants::GetAssetPath("sprites/candy-cane.png").c_str());

    ground_y_ = GetScreenHeight() - ground_.height;

    santa_frame_ = {0.0f, 0.0f,
                    (float)santa_walk_.width / Constants::kSantaWalkNumFrames,
                    (float)santa_walk_.height};

    // Set Santa to walking by default and update `current_santa_` when
    // the animation changes.
    current_santa_ = &santa_walk_;

    // Put Santa on the ground
    santa_position_ = {0.0f, (float)ground_y_ - santa_frame_.height + 20.0f};

    // Position candy cane off-screen
    candy_cane_position_ = {(float)GetScreenWidth() + GetRandomValue(100, 5000),
                            (float)ground_y_ - candy_cane_.height + 10};

    GenTextureMipmaps(&font_.texture);
    SetTextureFilter(font_.texture, TEXTURE_FILTER_POINT);
    restart_text_size_ = MeasureTextEx(menu_font_, kRestartText, kFontSize, 1);
    restart_text_position_ = {
        (GetScreenWidth() - restart_text_size_.x) / 2.0f,
        (GetScreenHeight() - restart_text_size_.y) / 2.0f + 100};

    quit_text_size_ = MeasureTextEx(menu_font_, kQuitText, kFontSize, 1);
    quit_text_position_ = {(GetScreenWidth() - quit_text_size_.x) / 2.0f,
                           (GetScreenHeight() - quit_text_size_.y) / 2.0f +
                               150};

    game_over_text_size_ = MeasureTextEx(font_, kGameOverText, 96, 1);
    game_over_text_position_ = {
        (GetScreenWidth() - game_over_text_size_.x) / 2.0f,
        (GetScreenHeight() - game_over_text_size_.y) / 2.0f};
}

GameScene::~GameScene() {
    UnloadFont(font_);
    UnloadFont(menu_font_);
    UnloadTexture(background_);
    UnloadTexture(ground_);
    UnloadTexture(santa_dead_);
    UnloadTexture(santa_walk_);
    UnloadTexture(candy_cane_);
}

SceneChangeRequest GameScene::update(float dt) {
    // Multiply by dt to ensure movement is time-based rather than frame-based
    if (!is_dead_ && frames_speed_ > 0.0f) {
        scrolling_back_ -= frames_speed_ * dt * kFrameMultiplier;
    }

    // Reset background scrolling if it has moved too far.
    if (scrolling_back_ <= -background_.width) {
        scrolling_back_ = 0;
    }

    // Handle input for movement
    if (!is_dead_ && !game_ended_ && (IsKeyDown(KEY_H) || IsKeyDown(KEY_O))) {
        // Increase speed smoothly
        frames_speed_ += kAcceleration * dt;
        if (frames_speed_ > kMaxFrameSpeed)
            frames_speed_ = (float)kMaxFrameSpeed;
    } else if (!is_animating_death_) {
        // If no keys are pressed, gradually decelerate
        frames_speed_ -= kDeceleration * dt;
        if (frames_speed_ < 0.0f)
            frames_speed_ = 0.0f;
    }

    if (!game_started_ && frames_speed_ > 0.0f) {
        game_started_ = true;
    }

    // Update animations if Santa moves or is animating death
    if (frames_speed_ > 0.0f || is_animating_death_) {
        frames_counter_ += Constants::kTargetFPS * dt * 3.0f;

        float frameDuration = (Constants::kTargetFPS /
                               (frames_speed_ > 0.0f ? frames_speed_ : 1.0f));
        if (frames_counter_ >= frameDuration || is_animating_death_) {
            frames_counter_ = 0.0f;
            current_frame_++;

            if (is_dead_) {
                // Death animation
                if (current_frame_ >= Constants::kSantaDeadNumFrames) {
                    current_frame_ = Constants::kSantaDeadNumFrames - 1;
                    is_animating_death_ = false;
                    if (!game_ended_) {
                        game_ended_ = true;
                        final_difference_ = current_difference_;
                    }
                }
            } else {
                // Walking animation
                if (current_frame_ >= Constants::kSantaWalkNumFrames) {
                    current_frame_ = 0;
                }
            }

            santa_frame_.x = (float)current_frame_ * santa_frame_.width;
        }

        if (!is_dead_) {
            candy_cane_position_.x -= frames_speed_ * dt * kFrameMultiplier;
        }
    }

    float santa_front_x = santa_position_.x + (santa_frame_.width / 2.0f) +
                          santa_frame_.width * 0.08f;
    float candy_cane_front_x =
        candy_cane_position_.x + (candy_cane_.width * 0.28f);
    current_difference_ = candy_cane_front_x - santa_front_x;

    if (!is_dead_ && santa_front_x >= candy_cane_front_x) {
        is_dead_ = true;
        is_animating_death_ = true;
        current_santa_ = &santa_dead_;
        santa_frame_ = {.x = 0.0f,
                        .y = 0.0f,
                        .width = (float)santa_dead_.width /
                                 Constants::kSantaDeadNumFrames,
                        .height = (float)santa_dead_.height};
        current_frame_ = 0;
    }

    // End game if Santa stops after starting
    if (game_started_ && frames_speed_ == 0.0f && !game_ended_ && !is_dead_) {
        game_ended_ = true;
        final_difference_ = current_difference_;
    }

    if (game_ended_) {
        Vector2 mouse_position = GetMousePosition();
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);

        Rectangle restart_text_bounds = {
            restart_text_position_.x, restart_text_position_.y,
            restart_text_size_.x, restart_text_size_.y};
        if (CheckCollisionPointRec(mouse_position, restart_text_bounds)) {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                return {Game, SceneChangeType::Transition};
            }
        }

        Rectangle quit_text_bounds = {quit_text_position_.x,
                                      quit_text_position_.y, quit_text_size_.x,
                                      quit_text_size_.y};
        if (CheckCollisionPointRec(mouse_position, quit_text_bounds)) {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                return {Title, SceneChangeType::Transition};
            }
        }
    }

    return {None, SceneChangeType::None};
}

void GameScene::draw() {
    ClearBackground(RAYWHITE);

    // Draw the scrolling background
    DrawTextureEx(background_, (Vector2){scrolling_back_, 0.0f}, 0.0f, 1.0f,
                  WHITE);
    DrawTextureEx(background_,
                  (Vector2){background_.width + scrolling_back_, 0.0f}, 0.0f,
                  1.0f, WHITE);

    DrawTextureEx(candy_cane_, candy_cane_position_, 0.0f, 1.0f, WHITE);

    for (int x = 0; x < GetScreenWidth(); x += ground_.width) {
        DrawTexture(ground_, x, ground_y_, WHITE);
    }

    DrawTexturePro(*current_santa_, santa_frame_,
                   {santa_position_.x, santa_position_.y, santa_frame_.width,
                    santa_frame_.height},
                   {0.0f, 0.0f}, 0.0f, WHITE);

#ifdef DEBUG
    float santa_front_x = santa_position_.x + (santa_frame_.width / 2.0f) +
                          santa_frame_.width * 0.08f;
    float candy_cane_front_x =
        candy_cane_position_.x + (candy_cane_.width * 0.28f);
    int santa_front_line_x = (int)santa_front_x;
    int candy_cane_front_line_x = (int)candy_cane_front_x;

    DrawLine(santa_front_line_x, 0, santa_front_line_x, GetScreenHeight(), RED);
    DrawText("Santa Front", santa_front_line_x + 5, 20, 20, RED);

    DrawLine(candy_cane_front_line_x, 0, candy_cane_front_line_x,
             GetScreenHeight(), BLUE);
    DrawText("CandyCane Front", candy_cane_front_line_x + 5, 40, 20, BLUE);
#endif

    if (game_ended_) {
        if (final_difference_ < 0.0f) {

            DrawTextEx(font_, kGameOverText, game_over_text_position_, 96, 1,
                       WHITE);
            DrawTextEx(menu_font_, kRestartText, restart_text_position_,
                       kFontSize, 1, WHITE);
            DrawTextEx(menu_font_, kQuitText, quit_text_position_, kFontSize, 1,
                       WHITE);
        } else {
            const char *game_over_score_text =
                TextFormat("Distance: %.2f", final_difference_);
            Vector2 game_over_score_text_size =
                MeasureTextEx(font_, game_over_score_text, 96, 1);
            Vector2 game_over_score_text_position = {
                (GetScreenWidth() - game_over_score_text_size.x) / 2.0f,
                (GetScreenHeight() - game_over_score_text_size.y) / 2.0f};

            DrawTextEx(font_, game_over_score_text,
                       game_over_score_text_position, 96, 1, WHITE);
            DrawTextEx(menu_font_, kRestartText, restart_text_position_,
                       kFontSize, 1, WHITE);
            DrawTextEx(menu_font_, kQuitText, quit_text_position_, kFontSize, 1,
                       WHITE);
        }
    }
}
