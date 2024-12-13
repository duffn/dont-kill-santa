#include <raylib.h>

#include "../../Constants.hpp"
#include "../Scenes.hpp"
#include "TitleScene.hpp"

using namespace std;

TitleScene::TitleScene() {
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    background_ = LoadTexture(
        Constants::GetAssetPath("backgrounds/background.png").c_str());
    title_font_ = LoadFontEx(
        Constants::GetAssetPath("fonts/hello-santa.ttf").c_str(), 96, 0, 0);
    menu_font_ = LoadFontEx(
        Constants::GetAssetPath("fonts/coolvetica.ttf").c_str(), 96, 0, 0);
    santa_dead_ =
        LoadTexture(Constants::GetAssetPath("sprites/santa-dead.png").c_str());

    GenTextureMipmaps(&title_font_.texture);
    SetTextureFilter(title_font_.texture, TEXTURE_FILTER_POINT);
    title_text_size_ =
        MeasureTextEx(title_font_, kTitleText, kTitleFontSize, 1);
    title_text_position_ = {(GetScreenWidth() - title_text_size_.x) / 2.0f,
                            (GetScreenHeight() - title_text_size_.y) / 2.0f -
                                150};

    GenTextureMipmaps(&menu_font_.texture);
    SetTextureFilter(menu_font_.texture, TEXTURE_FILTER_POINT);

    start_text_size_ =
        MeasureTextEx(menu_font_, kStartText, kSupplementalFontSize, 1);
    start_text_position_ = {(GetScreenWidth() - start_text_size_.x) / 2.0f,
                            (GetScreenHeight() - start_text_size_.y) / 2.0f +
                                150};

    instructions_text_size_ =
        MeasureTextEx(menu_font_, kInstructionsText, kSupplementalFontSize, 1);
    instructions_text_position_ = {
        (GetScreenWidth() - instructions_text_size_.x) / 2.0f,
        (GetScreenHeight() - instructions_text_size_.y) / 2.0f + 200};

    santa_frame_ = {0.0f, 0.0f,
                    (float)santa_dead_.width / Constants::kSantaDeadNumFrames,
                    (float)santa_dead_.height};
    santa_frame_.x = 16 * santa_frame_.width;
}

TitleScene::~TitleScene() {
    UnloadTexture(background_);
    UnloadFont(title_font_);
    UnloadFont(menu_font_);
    UnloadTexture(santa_dead_);
}

SceneChangeRequest TitleScene::update(float dt) {
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    Vector2 mouse_position = GetMousePosition();
    Rectangle instructions_text_bounds = {
        instructions_text_position_.x, instructions_text_position_.y,
        instructions_text_size_.x, instructions_text_size_.y};

    if (CheckCollisionPointRec(mouse_position, instructions_text_bounds)) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            return {Instructions, SceneChangeType::Change};
        }
    }

    Rectangle start_text_bounds = {start_text_position_.x,
                                   start_text_position_.y, start_text_size_.x,
                                   start_text_size_.y};
    if (CheckCollisionPointRec(mouse_position, start_text_bounds)) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            return {Game, SceneChangeType::Transition};
        }
    }

    return {None, SceneChangeType::None};
}

void TitleScene::draw() {
    ClearBackground(RAYWHITE);

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                  Constants::kBackgroundColor);
    DrawTextEx(title_font_, kTitleText, title_text_position_, kTitleFontSize, 1,
               WHITE);
    DrawTexturePro(santa_dead_, santa_frame_,
                   {title_text_position_.x - 20, title_text_position_.y + 10,
                    santa_frame_.width, santa_frame_.height},
                   {0.0f, 0.0f}, 0.0f, WHITE);
    DrawTextEx(menu_font_, kStartText, start_text_position_,
               kSupplementalFontSize, 1, WHITE);
    DrawTextEx(menu_font_, kInstructionsText, instructions_text_position_,
               kSupplementalFontSize, 1, WHITE);
}
