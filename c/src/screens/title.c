#include "config.h"
#include "raylib.h"
#include "screens.h"
#include <stdio.h>

static int frames_counter = 0;
static GameScreen finish_screen = NONE;
static float vs_alpha = 0.0f;
static float vs_scale = 0.0f;

static Vector2 mouse_position = {0.0f, 0.0f};
static char *instructions_text = "Instructions";
static Vector2 instructions_text_position = {0};
static Vector2 instructions_text_size = {0};

static const int supplemental_font_size = 36;

static char *start_text = "Start";
static Vector2 start_text_position = {0};
static Vector2 start_text_size = {0};

static Font font = {0};
static Font menu_font = {0};
static Vector2 text_size = {0};
static Vector2 text_position = {0};
static const char *text = "Don't Kill Santa";
static const int font_size = 96;
static const Color background_color = (Color){47, 79, 146, 255};

static Texture2D santa_dead = {0};
static Rectangle santa_frame = {0};

void init_title_screen(void) {
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    font = LoadFontEx("assets/fonts/hello-santa.ttf", 96, 0, 0);
    menu_font = LoadFontEx("assets/fonts/coolvetica.ttf", 96, 0, 0);
    santa_dead = LoadTexture("assets/sprites/santa-dead.png");

    GenTextureMipmaps(&font.texture);
    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);

    GenTextureMipmaps(&menu_font.texture);
    SetTextureFilter(menu_font.texture, TEXTURE_FILTER_POINT);
    text_size = MeasureTextEx(font, text, font_size, 1);
    text_position = (Vector2){(GetScreenWidth() - text_size.x) / 2.0f,
                              (GetScreenHeight() - text_size.y) / 2.0f - 150};

    start_text_size =
        MeasureTextEx(menu_font, start_text, supplemental_font_size, 1);
    start_text_position =
        (Vector2){(GetScreenWidth() - start_text_size.x) / 2.0f,
                  (GetScreenHeight() - start_text_size.y) / 2.0f + 150};

    instructions_text_size =
        MeasureTextEx(menu_font, instructions_text, supplemental_font_size, 1);
    instructions_text_position =
        (Vector2){(GetScreenWidth() - instructions_text_size.x) / 2.0f,
                  (GetScreenHeight() - instructions_text_size.y) / 2.0f + 200};

    finish_screen = NONE;

    santa_frame =
        (Rectangle){.x = 0.0f,
                    .y = 0.0f,
                    .width = (float)santa_dead.width / NUM_FRAMES_DEAD,
                    .height = (float)santa_dead.height};
    santa_frame.x = 16 * santa_frame.width;

    frames_counter = 0;
    vs_alpha = 0.0f;
    vs_scale = 10.0f;
}

void update_title_screen(void) {
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    mouse_position = GetMousePosition();

    if (CheckCollisionPointRec(mouse_position,
                               (Rectangle){instructions_text_position.x,
                                           instructions_text_position.y,
                                           instructions_text_size.x,
                                           instructions_text_size.y})) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            finish_screen = INSTRUCTIONS;
        }
    }

    if (CheckCollisionPointRec(
            mouse_position,
            (Rectangle){start_text_position.x, start_text_position.y,
                        start_text_size.x, start_text_size.y})) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            finish_screen = GAMEPLAY;
        }
    }
}

void draw_title_screen(void) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), background_color);
    DrawTextEx(font, text, text_position, font_size, 1, WHITE);
    DrawTexturePro(santa_dead, santa_frame,
                   (Rectangle){text_position.x - 20, text_position.y + 10,
                               santa_frame.width, santa_frame.height},
                   (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
    DrawTextEx(menu_font, start_text, start_text_position,
               supplemental_font_size, 1, WHITE);
    DrawTextEx(menu_font, instructions_text, instructions_text_position,
               supplemental_font_size, 1, WHITE);
}

void unload_title_screen(void) {
    UnloadFont(font);
    UnloadFont(menu_font);
    UnloadTexture(santa_dead);
}

GameScreen finish_title_screen(void) { return finish_screen; }
