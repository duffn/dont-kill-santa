#include "config.h"
#include "raylib.h"
#include "screens.h"
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

// Game state variables
static bool game_started = false;
static bool game_ended = false;
static bool is_dead = false;
static bool is_animating_death = false;

static GameScreen finish_screen = NONE;
static float frames_speed = 0.0f;

static int frames_counter = 0;
static int current_frame = 0;

static Rectangle santa_frame = {0};
static Texture2D santa_walk = {0};
static Texture2D santa_dead = {0};
static Texture2D current_santa = {0};
static Texture2D candy_cane = {0};
static Texture2D ground = {0};
static Texture2D background = {0};

static Font font = {0};
static Font menu_font = {0};
static const int font_size = 36;

static const char *restart_text = "Play Again";
static Vector2 restart_text_size = {0};
static Vector2 restart_text_position = {0};

static const char *quit_text = "Quit";
static Vector2 quit_text_size = {0};
static Vector2 quit_text_position = {0};

static Vector2 mouse_position = {0};

static Vector2 santa_position = {0};
static Vector2 candy_cane_position = {0};

static float distance = 0.0f;
static float final_distance = 0.0f;

static float scrolling_back = 0.0f;

static int ground_y = 0;

void init_gameplay_screen(void) {
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    santa_walk = LoadTexture("assets/sprites/santa-walk.png");
    santa_dead = LoadTexture("assets/sprites/santa-dead.png");
    candy_cane = LoadTexture("assets/sprites/candycane.png");
    ground = LoadTexture("assets/backgrounds/ground.png");
    background = LoadTexture("assets/backgrounds/background.png");
    font = LoadFontEx("assets/fonts/hello-santa.ttf", 96, 0, 0);
    menu_font = LoadFontEx("assets/fonts/coolvetica.ttf", 96, 0, 0);

    game_started = false;
    game_ended = false;
    is_dead = false;
    is_animating_death = false;
    finish_screen = NONE;
    distance = 0.0f;
    final_distance = 0.0f;

    // Randomly generate candy cane position off-screen
    const int candy_cane_position_off_screen = GetRandomValue(100, 5000);

    // Calculate ground position
    ground_y = GetScreenHeight() - ground.height;

    santa_frame =
        (Rectangle){.x = 0.0f,
                    .y = 0.0f,
                    .width = (float)santa_walk.width / NUM_FRAMES_WALK,
                    .height = (float)santa_walk.height};

    current_santa = santa_walk;

    // Place santa on the ground
    santa_position =
        (Vector2){0.0f, (float)ground_y - santa_frame.height + 20.0f};

    // Position candy cane off-screen
    candy_cane_position =
        (Vector2){(float)GetScreenWidth() + candy_cane_position_off_screen,
                  (float)ground_y - candy_cane.height + 10};

    GenTextureMipmaps(&font.texture);
    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
    restart_text_size = MeasureTextEx(menu_font, restart_text, font_size, 1);
    restart_text_position =
        (Vector2){(GetScreenWidth() - restart_text_size.x) / 2.0f,
                  (GetScreenHeight() - restart_text_size.y) / 2.0f + 100};

    quit_text_size = MeasureTextEx(menu_font, quit_text, font_size, 1);
    quit_text_position =
        (Vector2){(GetScreenWidth() - quit_text_size.x) / 2.0f,
                  (GetScreenHeight() - quit_text_size.y) / 2.0f + 150};
}

void update_gameplay_screen(float dt) {
    if (!is_dead && frames_speed > 0.0f) {
        scrolling_back -= frames_speed * dt * FRAME_MULTIPLIER;
    }

    // If the background has fully scrolled off to the left, reset the offset
    if (scrolling_back <= -background.width) {
        scrolling_back = 0;
    }

    if (!is_dead && !game_ended &&
        (IsKeyDown(KEY_H) || IsKeyDown(KEY_O) ||
         IsGestureDetected(GESTURE_HOLD) || IsGestureDetected(GESTURE_DRAG))) {
        // Increase speed smoothly
        frames_speed += FRAME_ACCELERATION * dt;
        if (frames_speed > MAX_FRAME_SPEED)
            frames_speed = (float)MAX_FRAME_SPEED;
    } else if (!is_animating_death) {
        // If no keys are pressed, gradually decelerate
        frames_speed -= FRAME_DECELERATION * dt;
        if (frames_speed < 0.0f)
            frames_speed = 0.0f;
    }

    // Check if game started
    if (!game_started && frames_speed > 0.0f) {
        game_started = true;
    }

    // Update animations if Santa moves or is animating death
    if (frames_speed > 0.0f || is_animating_death) {
        frames_counter += TARGET_FPS * dt * 3.0f;

        float frameDuration =
            (TARGET_FPS / (frames_speed > 0.0f ? frames_speed : 1.0f));
        if (frames_counter >= frameDuration || is_animating_death) {
            frames_counter = 0.0f;
            current_frame++;

            if (is_dead) {
                // Death animation
                if (current_frame >= NUM_FRAMES_DEAD) {
                    current_frame = NUM_FRAMES_DEAD - 1;
                    is_animating_death = false;
                    // Game ends after the death animation completes
                    if (!game_ended) {
                        game_ended = true;
                        final_distance = distance;
                    }
                }
            } else {
                // Walking animation
                if (current_frame >= NUM_FRAMES_WALK) {
                    current_frame = 0;
                }
            }

            santa_frame.x = (float)current_frame * santa_frame.width;
        }

        // Move the candy cane if Sant is not dead
        if (!is_dead) {
            candy_cane_position.x -= frames_speed * dt * FRAME_MULTIPLIER;
        }
    }

    float santa_front_x = santa_position.x + (santa_frame.width / 2.0f) +
                          santa_frame.width * 0.08f;
    float candy_cane_front_x =
        candy_cane_position.x + (candy_cane.width * 0.28f);
    distance = candy_cane_front_x - santa_front_x;

    // Check if santa passes the candy cane
    if (!is_dead && santa_front_x >= candy_cane_front_x) {
        is_dead = true;
        is_animating_death = true;
        current_santa = santa_dead;
        santa_frame =
            (Rectangle){.x = 0.0f,
                        .y = 0.0f,
                        .width = (float)santa_dead.width / NUM_FRAMES_DEAD,
                        .height = (float)santa_dead.height};

        // Start death animation from the first frame
        current_frame = 0;
    }

    // Check if game ends due to santa stopping after start
    if (game_started && frames_speed == 0 && !game_ended && !is_dead) {
        game_ended = true;
        final_distance = distance;
    }

    if (game_ended) {
        mouse_position = GetMousePosition();
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);

        if (CheckCollisionPointRec(
                mouse_position,
                (Rectangle){restart_text_position.x, restart_text_position.y,
                            restart_text_size.x, restart_text_size.y})) {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                finish_screen = GAMEPLAY;
            }
        }

        if (CheckCollisionPointRec(
                mouse_position,
                (Rectangle){quit_text_position.x, quit_text_position.y,
                            quit_text_size.x, quit_text_size.y})) {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                finish_screen = TITLE;
            }
        }
    }
}

void draw_gameplay_screen(void) {
    ClearBackground(RAYWHITE);

    // Draw the scrolling background
    DrawTextureEx(background, (Vector2){scrolling_back, 0.0f}, 0.0f, 1.0f,
                  WHITE);

    DrawTextureEx(background,
                  (Vector2){background.width + scrolling_back, 0.0f}, 0.0f,
                  1.0f, WHITE);

    DrawTextureEx(candy_cane, candy_cane_position, 0.0f, 1.0f, WHITE);

    for (int x = 0; x < GetScreenWidth(); x += ground.width) {
        DrawTexture(ground, x, ground_y, WHITE);
    }

    DrawTexturePro(current_santa, santa_frame,
                   (Rectangle){santa_position.x, santa_position.y,
                               santa_frame.width, santa_frame.height},
                   (Vector2){0.0f, 0.0f}, 0.0f, WHITE);

#ifdef DEBUG
    float santa_front_x = santa_position.x + (santa_frame.width / 2.0f) +
                          santa_frame.width * 0.08f;
    float candy_cane_front_x =
        candy_cane_position.x + (candy_cane.width * 0.28f);
    int santa_front_line_x = (int)santa_front_x;
    int candy_cane_front_line_x = (int)candy_cane_front_x;

    DrawLine(santa_front_line_x, 0, santa_front_line_x, GetScreenHeight(), RED);
    DrawText("santa front", santa_front_line_x + 5, 20, 20, RED);

    DrawLine(candy_cane_front_line_x, 0, candy_cane_front_line_x,
             GetScreenHeight(), BLUE);
    DrawText("candy_cane front", candy_cane_front_line_x + 5, 40, 20, BLUE);
#endif

    if (game_ended) {
        if (final_distance < 0.0f) {
            const char *game_over_text = "You Killed Santa";

            Vector2 game_over_text_size =
                MeasureTextEx(font, game_over_text, 96, 1);
            Vector2 game_over_text_position =
                (Vector2){(GetScreenWidth() - game_over_text_size.x) / 2.0f,
                          (GetScreenHeight() - game_over_text_size.y) / 2.0f};

            DrawTextEx(font, game_over_text, game_over_text_position, 96, 1,
                       WHITE);
            DrawTextEx(menu_font, restart_text, restart_text_position,
                       font_size, 1, WHITE);
            DrawTextEx(menu_font, quit_text, quit_text_position, font_size, 1,
                       WHITE);
        } else {
            const char *game_over_score_text =
                TextFormat("Distance: %.2f", final_distance);
            Vector2 game_over_score_text_size =
                MeasureTextEx(font, game_over_score_text, 96, 1);
            Vector2 game_over_score_text_position = (Vector2){
                (GetScreenWidth() - game_over_score_text_size.x) / 2.0f,
                (GetScreenHeight() - game_over_score_text_size.y) / 2.0f};

            DrawTextEx(font, game_over_score_text,
                       game_over_score_text_position, 96, 1, WHITE);
            DrawTextEx(menu_font, restart_text, restart_text_position,
                       font_size, 1, WHITE);
            DrawTextEx(menu_font, quit_text, quit_text_position, font_size, 1,
                       WHITE);
        }
    }
}

void unload_gameplay_screen(void) {
    UnloadFont(font);
    UnloadFont(menu_font);
    UnloadTexture(santa_walk);
    UnloadTexture(santa_dead);
    UnloadTexture(candy_cane);
    UnloadTexture(ground);
    UnloadTexture(background);
}

GameScreen finish_gameplay_screen(void) { return finish_screen; }
