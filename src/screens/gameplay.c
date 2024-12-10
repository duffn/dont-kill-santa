#include "config.h"
#include "raylib.h"
#include "screens.h"
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

// Game state variables
static bool gameStarted = false;
static bool gameEnded = false;
static bool isDead = false;
static bool isAnimatingDeath = false;

static int framesSpeed = 0;
static GameScreen finishScreen = -1;

static int framesCounter = 0;
static int currentFrame = 0;

static Rectangle santaFrame = {0};
static Texture2D santaWalk = {0};
static Texture2D santaDead = {0};
static Texture2D *currentSanta = {0};
static Texture2D candyCane = {0};
static Texture2D ground = {0};
static Texture2D background = {0};

static Font font = {0};
static Font menuFont = {0};
static const int fontSize = 36;

static const char *restartText = "Play Again";
static Vector2 restartTextSize = {0};
static Vector2 restartTextPosition = {0};

static const char *quitText = "Quit";
static Vector2 quitTextSize = {0};
static Vector2 quitTextPosition = {0};

static Vector2 mousePosition = {0};

static Vector2 santaPosition = {0};
static Vector2 candyCanePosition = {0};

static float difference = 0.0f;
static float finalDifference = 0.0f;

static float scrollingBack = 0.0f;

static int groundY = 0;

void InitGameplayScreen(void) {
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    santaWalk = LoadTexture("assets/sprites/santa-walk.png");
    santaDead = LoadTexture("assets/sprites/santa-dead.png");
    candyCane = LoadTexture("assets/sprites/candycane.png");
    ground = LoadTexture("assets/backgrounds/ground.png");
    background = LoadTexture("assets/backgrounds/background.png");
    font = LoadFontEx("assets/fonts/hello-santa.ttf", 96, 0, 0);
    menuFont = LoadFontEx("assets/fonts/coolvetica.ttf", 96, 0, 0);

    gameStarted = false;
    gameEnded = false;
    isDead = false;
    isAnimatingDeath = false;
    finishScreen = -1;
    difference = 0.0f;
    finalDifference = 0.0f;

    // Randomly generate candy cane position off-screen
    const int candyCanePositionOffScreen = GetRandomValue(500, 2500);

    // Calculate ground position
    groundY = SCREEN_HEIGHT - ground.height;

    santaFrame = (Rectangle){.x = 0.0f,
                             .y = 0.0f,
                             .width = (float)santaWalk.width / NUM_FRAMES_WALK,
                             .height = (float)santaWalk.height};

    currentSanta = &santaWalk;

    // Place Santa on the ground
    santaPosition = (Vector2){0.0f, (float)groundY - santaFrame.height + 20.0f};

    // Position candy cane off-screen
    candyCanePosition =
        (Vector2){(float)SCREEN_WIDTH + candyCanePositionOffScreen,
                  (float)groundY - candyCane.height + 10};

    GenTextureMipmaps(&font.texture);
    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
    restartTextSize = MeasureTextEx(menuFont, restartText, fontSize, 1);
    restartTextPosition =
        (Vector2){(GetScreenWidth() - restartTextSize.x) / 2.0f,
                  (GetScreenHeight() - restartTextSize.y) / 2.0f + 100};

    quitTextSize = MeasureTextEx(menuFont, quitText, fontSize, 1);
    quitTextPosition =
        (Vector2){(GetScreenWidth() - quitTextSize.x) / 2.0f,
                  (GetScreenHeight() - quitTextSize.y) / 2.0f + 150};
}

void UpdateGameplayScreen(void) {
    if (!isDead && framesSpeed > 0) {
        scrollingBack -= framesSpeed * 0.75f;
    }

    // If the background has fully scrolled off to the left, reset the offset
    if (scrollingBack <= -background.width) {
        scrollingBack = 0;
    }

    // If not dead, handle input for speed
    if (!isDead && !gameEnded && (IsKeyDown(KEY_H) || IsKeyDown(KEY_O))) {
        if (framesSpeed < MAX_FRAME_SPEED)
            framesSpeed++;
    } else if (!isAnimatingDeath) {
        if (framesSpeed > 0)
            framesSpeed--;
    }

    // Check if game started
    if (!gameStarted && framesSpeed > 0) {
        gameStarted = true;
    }

    // Update animations if Santa moves or animating death
    if (framesSpeed > 0 || isAnimatingDeath) {
        framesCounter++;

        if (framesCounter >= (TARGET_FPS / framesSpeed) || isAnimatingDeath) {
            framesCounter = 0;
            currentFrame++;

            if (isDead) {
                // Death animation
                if (currentFrame >= NUM_FRAMES_DEAD) {
                    currentFrame = NUM_FRAMES_DEAD - 1;
                    isAnimatingDeath = false;
                    // Game ends after death animation completes
                    if (!gameEnded) {
                        gameEnded = true;
                        finalDifference = difference;
                    }
                }
            } else {
                // Walking animation
                if (currentFrame >= NUM_FRAMES_WALK) {
                    currentFrame = 0;
                }
            }

            santaFrame.x = (float)currentFrame * santaFrame.width;
        }

        // Move candy cane if not dead
        if (!isDead) {
            candyCanePosition.x -= framesSpeed * 0.5f;
        }
    }

    // Calculate positions
    float santaFrontX =
        santaPosition.x + (santaFrame.width / 2.0f) + santaFrame.width * 0.08f;
    float candyCaneFrontX = candyCanePosition.x + (candyCane.width * 0.28f);
    difference = candyCaneFrontX - santaFrontX;

    // Check if Santa passes the candy cane
    if (!isDead && santaFrontX >= candyCaneFrontX) {
        isDead = true;
        isAnimatingDeath = true;
        currentSanta = &santaDead;
        santaFrame =
            (Rectangle){.x = 0.0f,
                        .y = 0.0f,
                        .width = (float)santaDead.width / NUM_FRAMES_DEAD,
                        .height = (float)santaDead.height};

        // Start death animation from the first frame
        currentFrame = 0;
    }

    // Check if game ends due to Santa stopping after start
    if (gameStarted && framesSpeed == 0 && !gameEnded && !isDead) {
        gameEnded = true;
        finalDifference = difference;
    }

    if (gameEnded) {
        mousePosition = GetMousePosition();
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);

        if (CheckCollisionPointRec(
                mousePosition,
                (Rectangle){restartTextPosition.x, restartTextPosition.y,
                            restartTextSize.x, restartTextSize.y})) {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                finishScreen = GAMEPLAY;
            }
        }

        if (CheckCollisionPointRec(
                mousePosition,
                (Rectangle){quitTextPosition.x, quitTextPosition.y,
                            quitTextSize.x, quitTextSize.y})) {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                finishScreen = TITLE;
            }
        }
    }
}

void DrawGameplayScreen(void) {
    ClearBackground(RAYWHITE);

    // Draw the scrolling background
    DrawTextureEx(background, (Vector2){scrollingBack, 0.0f}, 0.0f, 1.0f,
                  WHITE);

    DrawTextureEx(background, (Vector2){background.width + scrollingBack, 0.0f},
                  0.0f, 1.0f, WHITE);

    // Draw candy cane
    DrawTextureEx(candyCane, candyCanePosition, 0.0f, 1.0f, WHITE);

    // Draw ground
    for (int x = 0; x < SCREEN_WIDTH; x += ground.width) {
        DrawTexture(ground, x, groundY, WHITE);
    }

    // Draw Santa
    DrawTexturePro(*currentSanta, santaFrame,
                   (Rectangle){santaPosition.x, santaPosition.y,
                               santaFrame.width, santaFrame.height},
                   (Vector2){0.0f, 0.0f}, 0.0f, WHITE);

#ifdef DEBUG
    // Debug Lines
    float santaFrontX =
        santaPosition.x + (santaFrame.width / 2.0f) + santaFrame.width * 0.08f;
    float candyCaneFrontX = candyCanePosition.x + (candyCane.width * 0.28f);
    int santaFrontLineX = (int)santaFrontX;
    int candyCaneFrontLineX = (int)candyCaneFrontX;

    // Santa front line
    DrawLine(santaFrontLineX, 0, santaFrontLineX, SCREEN_HEIGHT, RED);
    DrawText("Santa Front", santaFrontLineX + 5, 20, 20, RED);

    // Candy cane front line
    DrawLine(candyCaneFrontLineX, 0, candyCaneFrontLineX, SCREEN_HEIGHT, BLUE);
    DrawText("CandyCane Front", candyCaneFrontLineX + 5, 40, 20, BLUE);
#endif

    // Display distance
    if (gameEnded) {
        if (finalDifference < 0.0f) {
            const char *gameOverText = "You killed Santa";

            Vector2 gameOverTextSize = MeasureTextEx(font, gameOverText, 96, 1);
            Vector2 gameOverTextPosition =
                (Vector2){(GetScreenWidth() - gameOverTextSize.x) / 2.0f,
                          (GetScreenHeight() - gameOverTextSize.y) / 2.0f};

            DrawTextEx(font, gameOverText, gameOverTextPosition, 96, 1, WHITE);
            DrawTextEx(menuFont, restartText, restartTextPosition, fontSize, 1,
                       WHITE);
            DrawTextEx(menuFont, quitText, quitTextPosition, fontSize, 1,
                       WHITE);
        } else {
            const char *gameOverScoreText =
                TextFormat("Distance: %.2f", finalDifference);
            Vector2 gameOverScoreTextSize =
                MeasureTextEx(font, gameOverScoreText, 96, 1);
            Vector2 gameOverScoreTextPosition =
                (Vector2){(GetScreenWidth() - gameOverScoreTextSize.x) / 2.0f,
                          (GetScreenHeight() - gameOverScoreTextSize.y) / 2.0f};

            DrawTextEx(font, gameOverScoreText, gameOverScoreTextPosition, 96,
                       1, WHITE);
            DrawTextEx(menuFont, restartText, restartTextPosition, fontSize, 1,
                       WHITE);
            DrawTextEx(menuFont, quitText, quitTextPosition, fontSize, 1,
                       WHITE);
        }
    }
}

void UnloadGameplayScreen(void) {
    UnloadFont(font);
    UnloadFont(menuFont);
    UnloadTexture(santaWalk);
    UnloadTexture(santaDead);
    UnloadTexture(candyCane);
    UnloadTexture(ground);
    UnloadTexture(background);
}

GameScreen FinishGameplayScreen(void) { return finishScreen; }
