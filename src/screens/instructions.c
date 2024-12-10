#include "raylib.h"
#include "screens.h"
#include <stdio.h>

static GameScreen finishScreen = -1;
static Font font = {0};
static Font instructionsFont = {0};
static Vector2 mousePosition = {0.0f, 0.0f};
static const Color backgroundColor = (Color){47, 79, 146, 255};

static Vector2 textPosition = {0};
static const char *text = "Instructions";
static const int titleFontSize = 72;

static char *playText = "Play";
static Vector2 playTextPosition = {0};
static Vector2 playTextSize = {0};
static const int supplementalFontSize = 36;

// Instructions Screen Initialization logic
void InitInstructionsScreen(void) {
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    font = LoadFontEx("assets/fonts/hello-santa.ttf", 96, 0, 0);
    instructionsFont = LoadFontEx("assets/fonts/coolvetica.ttf", 96, 0, 0);

    GenTextureMipmaps(&font.texture);
    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
    const Vector2 textSize = MeasureTextEx(font, text, titleFontSize, 1);
    textPosition = (Vector2){(GetScreenWidth() - textSize.x) / 2.0f, 10.0f};

    playTextSize =
        MeasureTextEx(GetFontDefault(), playText, supplementalFontSize, 1);
    playTextPosition =
        (Vector2){(GetScreenWidth() - playTextSize.x) / 2.0f,
                  (GetScreenHeight() - playTextSize.y) / 2.0f + 150};
    finishScreen = -1;
}

// Instructions Screen Update logic
void UpdateInstructionsScreen(void) {
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    mousePosition = GetMousePosition();

    if (CheckCollisionPointRec(
            mousePosition, (Rectangle){playTextPosition.x, playTextPosition.y,
                                       playTextSize.x, playTextSize.y})) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            finishScreen = GAMEPLAY;
        }
    }
}

// Instructions Screen Draw logic
void DrawInstructionsScreen(void) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), backgroundColor);
    DrawTextEx(font, text, textPosition, titleFontSize, 1,
               (Color){255, 255, 255, 255});
    const int fontSize = 24;
    const int startPositionY = 200;
    // Draw actual instructions
    DrawTextEx(instructionsFont,
               "Hold down H or O to make Santa run. HO, HO, HO!",
               (Vector2){10, startPositionY}, fontSize, 1, WHITE);
    DrawTextEx(instructionsFont, "Release the key to stop him.",
               (Vector2){10, startPositionY + 40}, fontSize, 1, WHITE);
    DrawTextEx(
        instructionsFont,
        "Stop as close as possible to the candy cane, but don't touch it "
        "or you know what happens!",
        (Vector2){10, startPositionY + 80}, fontSize, 1, WHITE);
    DrawTextEx(instructionsFont,
               "The closer you get to the candy cane, the better, so try for "
               "your lowest score!",
               (Vector2){10, startPositionY + 120}, fontSize, 1, WHITE);
    DrawTextEx(instructionsFont, playText, playTextPosition,
               supplementalFontSize, 1, WHITE);
}

// Instructions Screen Unload logic
void UnloadInstructionsScreen(void) {
    UnloadFont(font);
    UnloadFont(instructionsFont);
}

// Instructions Screen should finish?
GameScreen FinishInstructionsScreen(void) { return finishScreen; }
