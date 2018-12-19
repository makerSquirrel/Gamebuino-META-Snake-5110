#pragma once
#include <Arduino.h>

/** ******************   GLOBAL CONSTANTS: */
static const bool c_debug = false;
static const int8_t c_rasterX = 19;
static const int8_t c_rasterY = 15;
static const int8_t c_maxLevel = 9;
static const uint16_t g_arenaSize = (c_rasterX)*(c_rasterY);

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
#define PAUSE -1

/** ******************   GLOBAL VARIABLES: */
volatile static int8_t g_growNom; // standard nom +1, growNom will increase the snake length by 3
volatile static int16_t g_score;
volatile static int16_t g_cpuLoad; // for debugging
volatile static int16_t g_ramUsage; // for debugging
volatile static int8_t g_gameLevel; // max level 9
volatile static int8_t g_delayCounter;
volatile static int8_t g_lastButtonPressed; /// stores the last button that was pressed
volatile static int8_t g_lastTimeButtonPressed; ///  stores the button that was pressed in the previous time step (used for preventing kills by clicking backwards)
volatile static int8_t g_levelModuloHelper; // small helper to store the modulo value to handle game level
extern const uint8_t font5x7[];
extern const uint8_t font3x5[];
// only needed for non-classic mode:
volatile static bool g_xWallsRemoved[c_rasterX+1];
volatile static bool g_yWallsRemoved[c_rasterY+1];
