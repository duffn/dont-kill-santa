#include "config.h"
#include "raylib.h"
#include "screens.h"
#include <stdio.h>

static int framesCounter = 0;
static GameScreen finishScreen = -1;
static float vsAlpha = 0.0f;
static float vsScale = 0.0f;

static Vector2 mousePosition = {0.0f, 0.0f};
static char *instructionsText = "Instructions";
static Vector2 instructionsTextPosition = {0};
static Vector2 instructionsTextSize = {0};

static const int supplementalFontSize = 36;

static char *startText = "Start";
static Vector2 startTextPosition = {0};
static Vector2 startTextSize = {0};

static Font font = {0};
static Font menuFont = {0};
static Vector2 textSize = {0};
static Vector2 textPosition = {0};
static const char *text = "Don't Kill Santa";
static const int fontSize = 96;
static const Color backgroundColor = (Color){47, 79, 146, 255};

static Texture2D santaDead = {0};
static Rectangle santaFrame = {0};

// Title Screen Initialization logic
void InitTitleScreen(void) {
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    font = LoadFontEx("assets/fonts/hello-santa.ttf", 96, 0, 0);
    menuFont = LoadFontEx("assets/fonts/coolvetica.ttf", 96, 0, 0);
    santaDead = LoadTexture("assets/sprites/santa-dead.png");

    GenTextureMipmaps(&font.texture);
    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);

    GenTextureMipmaps(&menuFont.texture);
    SetTextureFilter(menuFont.texture, TEXTURE_FILTER_POINT);
    textSize = MeasureTextEx(font, text, fontSize, 1);
    textPosition = (Vector2){(GetScreenWidth() - textSize.x) / 2.0f,
                             (GetScreenHeight() - textSize.y) / 2.0f - 150};

    startTextSize = MeasureTextEx(menuFont, startText, supplementalFontSize, 1);
    startTextPosition =
        (Vector2){(GetScreenWidth() - startTextSize.x) / 2.0f,
                  (GetScreenHeight() - startTextSize.y) / 2.0f + 150};

    instructionsTextSize =
        MeasureTextEx(menuFont, instructionsText, supplementalFontSize, 1);
    instructionsTextPosition =
        (Vector2){(GetScreenWidth() - instructionsTextSize.x) / 2.0f,
                  (GetScreenHeight() - instructionsTextSize.y) / 2.0f + 200};

    finishScreen = -1;

    santaFrame = (Rectangle){.x = 0.0f,
                             .y = 0.0f,
                             .width = (float)santaDead.width / NUM_FRAMES_DEAD,
                             .height = (float)santaDead.height};
    santaFrame.x = 16 * santaFrame.width;

    framesCounter = 0;
    vsAlpha = 0.0f;
    vsScale = 10.0f;
}

// Title Screen Update logic
void UpdateTitleScreen(void) {
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    mousePosition = GetMousePosition();

    if (CheckCollisionPointRec(
            mousePosition,
            (Rectangle){instructionsTextPosition.x, instructionsTextPosition.y,
                        instructionsTextSize.x, instructionsTextSize.y})) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            finishScreen = INSTRUCTIONS;
        }
    }

    if (CheckCollisionPointRec(
            mousePosition, (Rectangle){startTextPosition.x, startTextPosition.y,
                                       startTextSize.x, startTextSize.y})) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            finishScreen = GAMEPLAY;
        }
    }
}

// Title Screen Draw logic
void DrawTitleScreen(void) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), backgroundColor);
    DrawTextEx(font, text, textPosition, fontSize, 1, WHITE);
    DrawTexturePro(santaDead, santaFrame,
                   (Rectangle){textPosition.x - 20, textPosition.y + 10,
                               santaFrame.width, santaFrame.height},
                   (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
    DrawTextEx(menuFont, startText, startTextPosition, supplementalFontSize, 1,
               WHITE);
    DrawTextEx(menuFont, instructionsText, instructionsTextPosition,
               supplementalFontSize, 1, WHITE);
}

// Title Screen Unload logic
void UnloadTitleScreen(void) {
    UnloadFont(font);
    UnloadFont(menuFont);
    UnloadTexture(santaDead);
}

// Title Screen should finish?
GameScreen FinishTitleScreen(void) { return finishScreen; }
