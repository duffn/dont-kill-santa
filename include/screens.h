#ifndef SCREENS_H
#define SCREENS_H

#include "raylib.h"

typedef enum GameScreen { TITLE = 0, GAMEPLAY, INSTRUCTIONS } GameScreen;

extern GameScreen currentScreen;
extern int score;
extern int result;
extern Music music;

// Title screen functions
void InitTitleScreen(void);
void UpdateTitleScreen(void);
void DrawTitleScreen(void);
void UnloadTitleScreen(void);
GameScreen FinishTitleScreen(void);

// Gameplay screen functions
void InitGameplayScreen(void);
void UpdateGameplayScreen(void);
void DrawGameplayScreen(void);
void UnloadGameplayScreen(void);
GameScreen FinishGameplayScreen(void);

// Instructions screen functions
void InitInstructionsScreen(void);
void UpdateInstructionsScreen(void);
void DrawInstructionsScreen(void);
void UnloadInstructionsScreen(void);
GameScreen FinishInstructionsScreen(void);

#endif // SCREENS_H
