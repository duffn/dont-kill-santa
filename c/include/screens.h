#ifndef SCREENS_H
#define SCREENS_H

#include "raylib.h"

typedef enum GameScreen { NONE = -1, TITLE, GAMEPLAY, INSTRUCTIONS } GameScreen;

extern GameScreen currentScreen;
extern int score;
extern int result;
extern Music music;

// Title screen functions
void init_title_screen(void);
void update_title_screen(void);
void draw_title_screen(void);
void unload_title_screen(void);
GameScreen finish_title_screen(void);

// Gameplay screen functions
void init_gameplay_screen(void);
void update_gameplay_screen(void);
void draw_gameplay_screen(void);
void unload_gameplay_screen(void);
GameScreen finish_gameplay_screen(void);

// Instructions screen functions
void init_instructions_screen(void);
void update_instructions_screen(void);
void draw_instructions_screen(void);
void unload_instructions_screen(void);
GameScreen finish_instructions_screen(void);

#endif // SCREENS_H
