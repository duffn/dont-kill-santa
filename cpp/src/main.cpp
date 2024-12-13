#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#include <raylib.h>

#include "audio/AudioManager.hpp"
#include "scenes/SceneManager.hpp"
#include "scenes/Scenes.hpp"
#include <Constants.hpp>

static void update_draw_frame();

int main() {
#if defined(DEBUG)
    SetTraceLogLevel(LOG_DEBUG);
#else
    SetTraceLogLevel(LOG_ERROR);
#endif

    InitWindow(Constants::kScreenWidth, Constants::kScreenHeight,
               Constants::kWindowTitle.c_str());

    SceneManager::initialize();
    SceneManager::set_current_scene(Scenes::Title);
    AudioManager::initialize("assets/sounds/music.wav", 0.5f);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(update_draw_frame, 60, 1);
#else
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        update_draw_frame();
    }
#endif

    AudioManager::cleanup();
    SceneManager::cleanup();
    CloseWindow();
    return 0;
}

void update_draw_frame() {
    float dt = GetFrameTime();

    AudioManager::update();
    SceneManager::update(dt);

    BeginDrawing();
    ClearBackground(WHITE);
    SceneManager::draw();

    if (SceneManager::is_on_transition()) {
        SceneManager::draw_transition();
    }

    EndDrawing();
}
