#include "config.h"
#include "raylib.h"
#include "screens.h"
#include <stdio.h>

static GameScreen finish_screen = NONE;
static Font font = {0};
static Font instructions_font = {0};
static Vector2 mouse_position = {0.0f, 0.0f};
static const Color background_color = (Color){47, 79, 146, 255};
static Texture2D background = {0};

static Texture2D santa_walk = {0};
static Rectangle santa_frame = {0};

static Vector2 text_position = {0};
static const char *text = "Instructions";
static const int title_font_size = 72;

static char *play_text = "Play";
static Vector2 play_text_position = {0};
static Vector2 play_text_size = {0};
static const int supplemental_font_size = 48;

void init_instructions_screen(void) {
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    santa_walk = LoadTexture("assets/sprites/santa-walk.png");
    background = LoadTexture("assets/backgrounds/background.png");
    font = LoadFontEx("assets/fonts/hello-santa.ttf", 96, 0, 0);
    instructions_font = LoadFontEx("assets/fonts/coolvetica.ttf", 96, 0, 0);

    GenTextureMipmaps(&font.texture);
    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
    const Vector2 text_size = MeasureTextEx(font, text, title_font_size, 1);
    text_position = (Vector2){(GetScreenWidth() - text_size.x) / 2.0f, 10.0f};

    play_text_size =
        MeasureTextEx(GetFontDefault(), play_text, supplemental_font_size, 1);
    play_text_position =
        (Vector2){(GetScreenWidth() - play_text_size.x) / 2.0f,
                  (GetScreenHeight() - play_text_size.y) / 2.0f + 175};

    santa_frame =
        (Rectangle){0.0f, 0.0f, (float)santa_walk.width / NUM_FRAMES_WALK,
                    (float)santa_walk.height};
    santa_frame.x = 4 * santa_frame.width;

    finish_screen = NONE;
}

void update_instructions_screen(float dt) {
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    mouse_position = GetMousePosition();

    if (CheckCollisionPointRec(mouse_position, (Rectangle){play_text_position.x,
                                                           play_text_position.y,
                                                           play_text_size.x,
                                                           play_text_size.y})) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            finish_screen = GAMEPLAY;
        }
    }
}

void draw_instructions_screen(void) {
    const int font_size = 24;
    const int start_position_y = 153;

    ClearBackground(RAYWHITE);
    DrawTextureEx(background, (Vector2){0.0f, 0.0f}, 0.0f, 1.0f, WHITE);

    DrawRectangle(0, start_position_y - 25, GetScreenWidth(), 200,
                  background_color);
    DrawTextEx(font, text, text_position, title_font_size, 1, WHITE);
    // draw actual instructions
    DrawTextEx(instructions_font,
               "Hold down H or O or hold down on a touchscreen to make santa "
               "run. HO, HO, HO!",
               (Vector2){10, start_position_y}, font_size, 1, WHITE);
    DrawTextEx(instructions_font,
               "Release the key or remove your finger from the touchscreen to "
               "stop him.",
               (Vector2){10, start_position_y + 40}, font_size, 1, WHITE);
    DrawTextEx(
        instructions_font,
        "Stop as close as possible to the candy cane, but don't touch it "
        "or you know what happens!",
        (Vector2){10, start_position_y + 80}, font_size, 1, WHITE);
    DrawTextEx(instructions_font,
               "The closer you get to the candy cane, the better, so try for "
               "your lowest score!",
               (Vector2){10, start_position_y + 120}, font_size, 1, WHITE);
    DrawTextEx(instructions_font, play_text, play_text_position,
               supplemental_font_size, 1, WHITE);
    DrawTexturePro(santa_walk, santa_frame,
                   (Rectangle){0.0f,
                               GetScreenHeight() - santa_frame.height - 10,
                               santa_frame.width, santa_frame.height},
                   (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
}

void unload_instructions_screen(void) {
    UnloadFont(font);
    UnloadFont(instructions_font);
    UnloadTexture(background);
}

GameScreen finish_instructions_screen(void) { return finish_screen; }
