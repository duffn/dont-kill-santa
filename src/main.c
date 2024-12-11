#include "config.h"
#include "raylib.h"
#include "screens.h"
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

GameScreen currentScreen = TITLE;
Music music = {0};
static float transAlpha = 0.0f;
static bool onTransition = false;
static bool transFadeOut = false;
static int transFromScreen = -1;
static int transToScreen = -1;

static void ChangeToScreen(GameScreen screen);
static void TransitionToScreen(GameScreen screen);
static void UpdateTransition(void);
static void DrawTransition(void);
static void UpdateDrawFrame(void);

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Don't kill Santa");

    currentScreen = TITLE;
    InitTitleScreen();

    InitAudioDevice();
    music = LoadMusicStream("assets/sounds/music.wav");
    SetMusicVolume(music, 0.5f);
    PlayMusicStream(music);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(TARGET_FPS);

    while (!WindowShouldClose()) {
        UpdateDrawFrame();
    }
#endif

    switch (currentScreen) {
    case TITLE:
        UnloadTitleScreen();
        break;
    case GAMEPLAY:
        UnloadGameplayScreen();
        break;
    case INSTRUCTIONS:
        UnloadInstructionsScreen();
        break;
    default:
        break;
    }

    UnloadMusicStream(music);
    CloseAudioDevice();

    CloseWindow();

    return 0;
}

static void TransitionToScreen(GameScreen screen) {
    onTransition = true;
    transFadeOut = false;
    transFromScreen = currentScreen;
    transToScreen = screen;
    transAlpha = 0.0f;
}

// Update transition effect
static void UpdateTransition(void) {
    if (!transFadeOut) {
        transAlpha += 0.02f;

        // NOTE: Due to float internal representation, condition jumps on 1.0f
        // instead of 1.05f For that reason we compare against 1.01f, to avoid
        // last frame loading stop
        if (transAlpha > 1.01f) {
            transAlpha = 1.0f;

            // Unload current screen
            switch (transFromScreen) {
            case TITLE:
                UnloadTitleScreen();
                break;
            case GAMEPLAY:
                UnloadGameplayScreen();
                break;
            case INSTRUCTIONS:
                UnloadInstructionsScreen();
                break;
            default:
                break;
            }

            // Load next screen
            switch (transToScreen) {
            case TITLE:
                InitTitleScreen();
                break;
            case GAMEPLAY:
                InitGameplayScreen();
                break;
            case INSTRUCTIONS:
                InitInstructionsScreen();
                break;
            default:
                break;
            }

            currentScreen = transToScreen;

            // Activate fade out effect to next loaded screen
            transFadeOut = true;
        }
    } else {
        transAlpha -= 0.02f;

        if (transAlpha < -0.01f) {
            transAlpha = 0.0f;
            transFadeOut = false;
            onTransition = false;
            transFromScreen = -1;
            transToScreen = -1;
        }
    }
}

// Draw transition effect (full-screen rectangle)
static void DrawTransition(void) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                  Fade(BLACK, transAlpha));
}

// Change to screen, no transition effect
static void ChangeToScreen(GameScreen screen) {
    switch (currentScreen) {
    case TITLE:
        UnloadTitleScreen();
        break;
    case GAMEPLAY:
        UnloadGameplayScreen();
        break;
    case INSTRUCTIONS:
        UnloadInstructionsScreen();
        break;
    default:
        break;
    }

    switch (screen) {
    case TITLE:
        InitTitleScreen();
        break;
    case GAMEPLAY:
        InitGameplayScreen();
        break;
    case INSTRUCTIONS:
        InitInstructionsScreen();
        break;
    default:
        break;
    }

    currentScreen = screen;
}

// Update and draw game frame
static void UpdateDrawFrame(void) {
    UpdateMusicStream(music);

    if (!onTransition) {
        switch (currentScreen) {
        case TITLE: {
            UpdateTitleScreen();

            if (FinishTitleScreen() == GAMEPLAY) {
                TransitionToScreen(GAMEPLAY);
            }
            if (FinishTitleScreen() == INSTRUCTIONS) {
                ChangeToScreen(INSTRUCTIONS);
            }
        } break;
        case GAMEPLAY: {
            UpdateGameplayScreen();

            if (FinishGameplayScreen() == TITLE)
                TransitionToScreen(TITLE);
            if (FinishGameplayScreen() == GAMEPLAY)
                TransitionToScreen(GAMEPLAY);
        } break;
        case INSTRUCTIONS: {
            UpdateInstructionsScreen();

            if (FinishInstructionsScreen() == GAMEPLAY)
                TransitionToScreen(GAMEPLAY);
        } break;
        default:
            break;
        }
    } else
        UpdateTransition();

    BeginDrawing();

    ClearBackground(RAYWHITE);

    switch (currentScreen) {
    case TITLE:
        DrawTitleScreen();
        break;
    case GAMEPLAY:
        DrawGameplayScreen();
        break;
    case INSTRUCTIONS:
        DrawInstructionsScreen();
        break;
    default:
        break;
    }

    // Draw full screen rectangle in front of everything
    if (onTransition)
        DrawTransition();

    EndDrawing();
}
