#include <raylib.h>

#include "../../Constants.hpp"
#include "../Scenes.hpp"
#include "InstructionsScene.hpp"

InstructionsScene::InstructionsScene() {
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    background_ = LoadTexture(
        Constants::GetAssetPath("backgrounds/background.png").c_str());
    title_font_ = LoadFontEx(
        Constants::GetAssetPath("fonts/hello-santa.ttf").c_str(), 96, 0, 0);
    instructions_font_ = LoadFontEx(
        Constants::GetAssetPath("fonts/coolvetica.ttf").c_str(), 96, 0, 0);
    santa_walk_ =
        LoadTexture(Constants::GetAssetPath("sprites/santa-walk.png").c_str());

    GenTextureMipmaps(&title_font_.texture);
    SetTextureFilter(title_font_.texture, TEXTURE_FILTER_POINT);
    title_text_size_ =
        MeasureTextEx(title_font_, kTitleText, kTitleFontSize, 1);
    title_text_position_ = {(GetScreenWidth() - title_text_size_.x) / 2.0f,
                            10.0f};

    GenTextureMipmaps(&instructions_font_.texture);
    SetTextureFilter(instructions_font_.texture, TEXTURE_FILTER_POINT);

    play_text_size_ =
        MeasureTextEx(GetFontDefault(), kPlayText, kSupplementalFontSize, 1);
    play_text_position_ = {(GetScreenWidth() - play_text_size_.x) / 2.0f,
                           (GetScreenHeight() - play_text_size_.y) / 2.0f +
                               175};

    santa_frame_ = {0.0f, 0.0f,
                    (float)santa_walk_.width / Constants::kSantaWalkNumFrames,
                    (float)santa_walk_.height};
    santa_frame_.x = 4 * santa_frame_.width;
}

InstructionsScene::~InstructionsScene() {
    UnloadTexture(background_);
    UnloadFont(title_font_);
    UnloadFont(instructions_font_);
    UnloadTexture(santa_walk_);
}

SceneChangeRequest InstructionsScene::update(float dt) {
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    Vector2 mouse_position = GetMousePosition();
    Rectangle play_text_bounds = {play_text_position_.x, play_text_position_.y,
                                  play_text_size_.x, play_text_size_.y};

    if (CheckCollisionPointRec(mouse_position, play_text_bounds)) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            return {Game, SceneChangeType::Transition};
        }
    }

    return {None, SceneChangeType::None};
}

void InstructionsScene::draw() {
    constexpr int start_position_y = 153;
    constexpr int font_size = 24;

    ClearBackground(RAYWHITE);
    DrawTextureEx(background_, {0.0f, 0.0f}, 0.0f, 1.0f, WHITE);

    DrawRectangle(0, start_position_y - 25, GetScreenWidth(), 200,
                  Constants::kBackgroundColor);
    DrawTextEx(title_font_, kTitleText, title_text_position_, kTitleFontSize, 1,
               WHITE);

    // Draw actual instructions
    DrawTextEx(instructions_font_,
               "Hold down H or O or hold down on a touchscreen to make Santa "
               "run. HO, HO, HO!",
               {10, start_position_y}, font_size, 1, WHITE);
    DrawTextEx(instructions_font_,
               "Release the key or remove your finger from the touchscreen to "
               "stop him.",
               {10, start_position_y + 40}, font_size, 1, WHITE);
    DrawTextEx(
        instructions_font_,
        "Stop as close as possible to the candy cane, but don't touch it "
        "or you know what happens!",
        {10, start_position_y + 80}, font_size, 1, WHITE);
    DrawTextEx(instructions_font_,
               "The closer you get to the candy cane, the better, so try for "
               "your lowest score!",
               {10, start_position_y + 120}, font_size, 1, WHITE);

    // Play button and Santa
    DrawTextEx(instructions_font_, kPlayText, play_text_position_,
               kSupplementalFontSize, 1, WHITE);
    DrawTexturePro(santa_walk_, santa_frame_,
                   {0.0f, GetScreenHeight() - santa_frame_.height - 10,
                    santa_frame_.width, santa_frame_.height},
                   {0.0f, 0.0f}, 0.0f, WHITE);
}
