#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <time.h>

#define uint unsigned int

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 600

#define TILE_WIDTH 32
#define TILE_HEIGHT 32

#define TEXT_BITMAP_ROWS 16
#define TEXT_BITMAP_COLS 16

#define NUM_TOTAL_TEXTURES 5
#define GRAVITY_INCREMENT 0.35
#define XBALL_AIR_RESISTANCE 0.001
#define XBALL_GROUND_RESISTANCE 0.02
#define XPLAYER_AIR_RESISTANCE 0.0
#define XPLAYER_GROUND_RESISTANCE 0.3
#define END_BALL_Y_VELOCITY 0.275
#define END_BALL_X_VELOCITY 0.025
#define END_PLAYER_X_VELOCITY 0.25
#define BACKBOARD_DAMPENING_X_FACTOR 0.8
#define BACKBOARD_DAMPENING_Y_FACTOR 0.9
#define FLOOR_DAMPENING_FACTOR 7.0/9.0
#define NET_ANIMATION_TICK_DELAY 6
#define RIM_DAMPENING_X_OFFSET 0.15
#define RIM_DAMPENING_X_FACTOR 0.25
#define RIM_DAMPENING_Y_FACTOR 0.45
#define SCORE1_LOC_Y 362.0
#define SCORE1_LOC_X 38.0
#define SCORE2_LOC_Y 362.0
#define SCORE2_LOC_X 961.0
#define SCORE_REGION_HALF_LENGTH 14.0
#define SCORE_REGION_HALF_HEIGHT 8.0
#define MAX_VELOCITY 20.0
#define WINDOW_OFFSET 7.0
#define PLAYER1_HOLD_POSITION_OFFSET 14.0
#define SHOOT_X_DAMPEN_FACTOR 57.0
#define SHOOT_Y_DAMPEN_FACTOR 19.0
#define BALL_RADIUS 9.0
#define THREE_POINT_LINE_LOC1 700.0
#define THREE_POINT_LINE_LOC2 300.0
#define NET1_LOC_X 0.0
#define NET2_LOC_X 937.0
#define NET1_LOC_Y 305.0
#define NET2_LOC_Y 305.0
#define PLAYER1_START_Y 457.0
#define PLAYER2_START_Y 457.0
#define PLAYER1_START_X 778.0
#define PLAYER2_START_X 200.0
#define BACKBOARD_LENGTH 51.0
#define BACKBOARD_X_LOC 305.0
#define AI_BALL_APPROACH_THRESHOLD 20.0
#define RIM_EDGE_CIRCLE_RADIUS 1.8
#define END_SCORE 999999999
#define MAX_GAME_TIME 303.0
#define MAX_OT_TIME 120.0
#define PLAYERMOVE_ANIMATION_TICK_DELAY 9

#define TOTAL_GAME_TIME(p) (MAX_GAME_TIME + (MAX_OT_TIME * p))
#define TOTAL_TIME_REMAINING(p,e) (TOTAL_GAME_TIME(p) - e)

static const SDL_Color color_light_gray = {192,192,192,0};
static const SDL_Color color_white = {255,255,255,0};
static const SDL_Color color_yellow = {255,255,0,0};
static const SDL_Color color_red = {255,0,0,0};

enum obj_type
{
    OBJ_BACKGROUND,
    OBJ_BASKETBALL,
    OBJ_NET,
    OBJ_PLAYER1,
    OBJ_PLAYER2
};

enum playerevent_state
{
    PEVENT_NONE,
    PEVENT_PLAYER1,
    PEVENT_PLAYER2
};

enum move_state
{
    MOVEST_NONE,
    MOVEST_LEFT,
    MOVEST_RIGHT
};

#endif
