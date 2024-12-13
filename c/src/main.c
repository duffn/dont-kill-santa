#include "config.h"
#include "raylib.h"
#include "screens.h"
#if defined(platform_WEB)
#include <emscripten/emscripten.h>
#endif

GameScreen current_screen = TITLE;
Music music = {0};
static float trans_alpha = 0.0f;
static bool on_transition = false;
static bool trans_fade_out = false;
static int trans_from_screen = -1;
static int trans_to_screen = -1;

static void change_to_screen(GameScreen screen);
static void transition_to_screen(GameScreen screen);
static void update_transition(void);
static void draw_transition(void);
static void update_draw_frame(void);

int main(void) {
#ifdef debug
    SetTraceLogLevel(LOG_DEBUG);
#else
    SetTraceLogLevel(LOG_ERROR);
#endif

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Don't Kill Santa");

    current_screen = TITLE;
    init_title_screen();

    InitAudioDevice();
    music = LoadMusicStream("assets/sounds/music.wav");
    SetMusicVolume(music, 0.5f);
    PlayMusicStream(music);

#if defined(platform_WEB)
    emscripten_set_main_loop(update_draw_frame, 60, 1);
#else
    SetTargetFPS(TARGET_FPS);

    while (!WindowShouldClose()) {
        update_draw_frame();
    }
#endif

    switch (current_screen) {
    case TITLE:
        unload_title_screen();
        break;
    case GAMEPLAY:
        unload_gameplay_screen();
        break;
    case INSTRUCTIONS:
        unload_instructions_screen();
        break;
    default:
        break;
    }

    UnloadMusicStream(music);
    CloseAudioDevice();

    CloseWindow();

    return 0;
}

static void transition_to_screen(GameScreen screen) {
    on_transition = true;
    trans_fade_out = false;
    trans_from_screen = current_screen;
    trans_to_screen = screen;
    trans_alpha = 0.0f;
}

// update transition effect
static void update_transition(void) {
    if (!trans_fade_out) {
        trans_alpha += 0.02f;

        // note: due to float internal representation, condition jumps on 1.0f
        // instead of 1.05f for that reason we compare against 1.01f, to avoid
        // last frame loading stop
        if (trans_alpha > 1.01f) {
            trans_alpha = 1.0f;

            // unload current screen
            switch (trans_from_screen) {
            case TITLE:
                unload_title_screen();
                break;
            case GAMEPLAY:
                unload_gameplay_screen();
                break;
            case INSTRUCTIONS:
                unload_instructions_screen();
                break;
            default:
                break;
            }

            // load next screen
            switch (trans_to_screen) {
            case TITLE:
                init_title_screen();
                break;
            case GAMEPLAY:
                init_gameplay_screen();
                break;
            case INSTRUCTIONS:
                init_instructions_screen();
                break;
            default:
                break;
            }

            current_screen = trans_to_screen;

            // activate fade out effect to next loaded screen
            trans_fade_out = true;
        }
    } else {
        trans_alpha -= 0.02f;

        if (trans_alpha < -0.01f) {
            trans_alpha = 0.0f;
            trans_fade_out = false;
            on_transition = false;
            trans_from_screen = -1;
            trans_to_screen = -1;
        }
    }
}

// draw transition effect (full-screen rectangle)
static void draw_transition(void) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                  Fade(BLACK, trans_alpha));
}

// change to screen, no transition effect
static void change_to_screen(GameScreen screen) {
    switch (current_screen) {
    case TITLE:
        unload_title_screen();
        break;
    case GAMEPLAY:
        unload_gameplay_screen();
        break;
    case INSTRUCTIONS:
        unload_instructions_screen();
        break;
    default:
        break;
    }

    switch (screen) {
    case TITLE:
        init_title_screen();
        break;
    case GAMEPLAY:
        init_gameplay_screen();
        break;
    case INSTRUCTIONS:
        init_instructions_screen();
        break;
    default:
        break;
    }

    current_screen = screen;
}

// update and draw game frame
static void update_draw_frame(void) {
    UpdateMusicStream(music);

    if (!on_transition) {
        switch (current_screen) {
        case TITLE: {
            update_title_screen();

            if (finish_title_screen() == GAMEPLAY) {
                transition_to_screen(GAMEPLAY);
            }
            if (finish_title_screen() == INSTRUCTIONS) {
                change_to_screen(INSTRUCTIONS);
            }
        } break;
        case GAMEPLAY: {
            update_gameplay_screen();

            if (finish_gameplay_screen() == TITLE)
                transition_to_screen(TITLE);
            if (finish_gameplay_screen() == GAMEPLAY)
                transition_to_screen(GAMEPLAY);
        } break;
        case INSTRUCTIONS: {
            update_instructions_screen();

            if (finish_instructions_screen() == GAMEPLAY)
                transition_to_screen(GAMEPLAY);
        } break;
        default:
            break;
        }
    } else
        update_transition();

    BeginDrawing();

    ClearBackground(RAYWHITE);

    switch (current_screen) {
    case TITLE:
        draw_title_screen();
        break;
    case GAMEPLAY:
        draw_gameplay_screen();
        break;
    case INSTRUCTIONS:
        draw_instructions_screen();
        break;
    default:
        break;
    }

    // draw full screen rectangle in front of everything
    if (on_transition)
        draw_transition();

    EndDrawing();
}
