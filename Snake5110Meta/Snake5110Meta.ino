#include <Gamebuino-Meta.h>
#include "globalVariables.h"
#include "coordinate.h"
#include "config-gamebuino.h"
#include "snake.h"
#include "menuimages.h"
#include "highscore.h"
#include "soundfx.h"

Snake g_snake;

/// CC-BY-SA 2018 by Lady Awesome and MakerSquirrel, original Highscore code by Rodot (here a modified version in use)

/** ******************   GLOBAL SHIT: */
static bool g_isClassicMode; // always set when game starts
static uint8_t g_xWallsRemaining;
static uint8_t g_yWallsRemaining;
/// classic mode
// #define BGCOLORCLASSICBLUE gb.createColor(206, 221, 231)
struct CustomColors {
  static const Color s_bgNokiaGreen;
  static const Color s_bgClassicBlue;
  static const Color s_bgNokiaBlue;
};
const Color CustomColors::s_bgNokiaGreen = gb.createColor(185, 199, 0);
const Color CustomColors::s_bgClassicBlue = gb.createColor(206, 221, 231);
const Color CustomColors::s_bgNokiaBlue = gb.createColor(158, 170, 197);
// Color g_colorNom;
Color g_colorNom = Color(BLACK);



HighScore g_classicHighscore = HighScore("Classic", g_colorNom, CustomColors::s_bgNokiaGreen);
#define SAVE_CLASSIC 0
HighScore g_metaHighscore = HighScore("Meta", g_colorNom, CustomColors::s_bgClassicBlue);
#define SAVE_META 1
HighScore g_snake2Highscore = HighScore("Wall-less", g_colorNom, CustomColors::s_bgNokiaBlue);
#define SAVE_SNAKE2 3

/// used to store save-able stuff.
struct GameSettings {
  Color bgColor;
  Color wallColor;
  int8_t level;/// TODO: measure orignal snake speed with camera
  int8_t bgColorType; // classic blue, nokia green and nokia blue
  int8_t wallColorType; // orange, grey.
  bool snakeStyle; // TODO: implement graphics and settings. false: like Snake 1, true: like Snake 2
  bool allowSpeedUp; /// if true, holding the arrow key sets speed of snake to max until key release. TODO: set via settings menu
  bool alwaysShowScore; // if true, score is always shown, TODO: set via settings menu
  bool initialized; // used to determine if settings were loaded
  GameSettings() :
    bgColor(CustomColors::s_bgClassicBlue),
    wallColor(ORANGE),
    level(5), bgColorType(0), wallColorType(0),
    snakeStyle(false), allowSpeedUp(false), alwaysShowScore(false), initialized(true) {}
  void setBgColor(const Color& newColor) { bgColor = newColor; }
  void setBgColorByType(int8_t newType)
  {
    if(newType < 0 || newType > 2) return;
    bgColorType = newType;
    if (newType == 0)
      bgColor = CustomColors::s_bgClassicBlue;
    else if (newType == 1)
      bgColor = CustomColors::s_bgNokiaGreen;
    else if (newType == 2)
      bgColor = CustomColors::s_bgNokiaBlue;
  }

  void setWallColor(const Color& newColor) { wallColor = newColor; }
  void setWallColorByType(int8_t newType)
  {
    if(newType < 0 || newType > 2) return;
    wallColorType = newType;
    if (newType == 0)
      wallColor = ORANGE;
    else if (newType == 1)
      wallColor = GRAY;
  }
};

GameSettings g_gameSettings = GameSettings();
#define SAVE_SETTINGS 4

const SaveDefault savefileDefaults[] = {
  { SAVE_CLASSIC, SAVETYPE_BLOB, sizeof(g_classicHighscore), 0 },
  { SAVE_META, SAVETYPE_BLOB, sizeof(g_metaHighscore), 0 },
  { 2, SAVETYPE_INT, 1, 0 },
  { SAVE_SNAKE2, SAVETYPE_BLOB, sizeof(g_snake2Highscore), 0 },
  { SAVE_SETTINGS, SAVETYPE_BLOB, sizeof(g_gameSettings), 0 }
};

const SaveDefault oldSavefileDefaults[] = {
  { SAVE_CLASSIC, SAVETYPE_BLOB, sizeof(g_classicHighscore), 0 },
  { SAVE_META, SAVETYPE_BLOB, sizeof(g_metaHighscore), 0 }
};


/// game-mode-specific stuff, which is runtime relevant only
struct GameMode {
  enum Mode { Classic, Meta, Wallless};

  Mode mode;

};

GameMode g_gameMode = GameMode();

/** ******************   FUNCTION DEFINITIONS: */


bool isXWallRemoved(int8_t val) {
    if (val < 0 || val >= c_rasterX)
        return false;
    return g_xWallsRemoved[val];
}


bool isYWallRemoved(int8_t val) {
    if (val < 0 || val >= c_rasterY)
        return false;
    return g_yWallsRemoved[val];
}


/// check if out of bounds (not in arena and not in the walls)
bool isOutOfBounds(const Coordinate& pos) {
    if (pos.isInArena())
        return false;
    if (g_isClassicMode)
        return true;
    if (g_gameMode.mode == GameMode::Classic)
        return true;
    if (pos.x() < -1 || pos.x() > c_rasterX)
        return true;
    if (pos.y() < -1 || pos.y() > c_rasterY)
        return true;
    if (g_gameMode.mode == GameMode::Wallless)
        return false;
    if (isXWallRemoved(pos.x()) || isYWallRemoved(pos.y()))
        return false;
    return true;
}


void deleteRandomWallElement() {
  if (g_xWallsRemaining == 0 && g_yWallsRemaining == 0)
    return;
  if (g_xWallsRemaining == 0 && g_yWallsRemaining == 1)
  {
    /// remove last y wall
    for(auto i = 0; i < c_rasterY+1; i++)
    {
      if (!g_yWallsRemoved[i])
        continue;
      g_yWallsRemoved[i] = true;
      g_yWallsRemaining = 0;
      return;
    }
  }
  if (g_xWallsRemaining == 1 && g_yWallsRemaining == 0)
  {
    /// remove last x wall
    for(auto i = 0; i < c_rasterX+1; i++)
    {
      if (!g_xWallsRemoved[i])
        continue;
      g_xWallsRemoved[i] = true;
      g_xWallsRemaining = 0;
      return;
    }
  }
  bool inUse = true;
  while(inUse)
  {
    uint8_t rngXElement = random(0, c_rasterX+1);
    uint8_t rngYElement = random(0, c_rasterY+1);
    bool removedY = g_yWallsRemoved[rngYElement];
    bool removedX = g_xWallsRemoved[rngXElement];
    if (!removedY)
    {
      inUse = false;
      g_yWallsRemoved[rngYElement] = true;
      g_yWallsRemaining--;
    }
    else if (!removedX)
    {
      inUse = false;
      g_xWallsRemoved[rngXElement] = true;
      g_xWallsRemaining--;
    }
  }
}


/// setup function
void setup() {
  gb.begin();

  g_cpuLoad = 0;
  g_ramUsage = 0;
  gb.display.init(80, 64, ColorMode::rgb565);
  gb.tft.setCursor(0, 20);
	gb.tft.setColor(Color::red, Color::black);
	gb.tft.print("setupBegin");
  gb.display.setColor(BLACK, CustomColors::s_bgClassicBlue);

  if (gb.save.get(2) == 1) // new file system
  {
    gb.tft.setCursor(0, 30);
  	gb.tft.setColor(Color::red, Color::black);
  	gb.tft.print(" saveGet 2 == 1");
    gb.save.config(savefileDefaults);
    gb.save.get(SAVE_CLASSIC, g_classicHighscore);
    g_classicHighscore.setName("Classic");
    g_classicHighscore.setTextColor(g_colorNom);
    g_classicHighscore.setBgColor(CustomColors::s_bgNokiaGreen);
    gb.save.get(SAVE_META, g_metaHighscore);
    g_metaHighscore.setName("   Meta");
    g_metaHighscore.setTextColor(g_colorNom);
    g_metaHighscore.setBgColor(CustomColors::s_bgClassicBlue);
    gb.save.get(SAVE_SETTINGS, g_gameSettings);
    if (!g_gameSettings.initialized)
      { g_gameSettings = GameSettings(); }
    gb.save.get(SAVE_SNAKE2, g_snake2Highscore);
    g_snake2Highscore.setName("Wall-less");
    g_snake2Highscore.setTextColor(g_colorNom);
    g_snake2Highscore.setBgColor(CustomColors::s_bgNokiaBlue);
  }
  else
  {
    gb.tft.setCursor(0, 40);
    gb.tft.setColor(Color::red, Color::black);
    gb.tft.print(" saveGet 2 != 1");
    gb.save.config(oldSavefileDefaults);
    gb.save.get(SAVE_CLASSIC, g_classicHighscore);
    g_classicHighscore.setName("Classic");
    g_classicHighscore.setTextColor(g_colorNom);
    g_classicHighscore.setBgColor(CustomColors::s_bgNokiaGreen);
    gb.save.get(SAVE_META, g_metaHighscore);
    g_metaHighscore.setName("   Meta");
    g_metaHighscore.setTextColor(g_colorNom);
    g_metaHighscore.setBgColor(CustomColors::s_bgClassicBlue);
    gb.tft.setCursor(33, 40);
    gb.tft.setColor(Color::red, Color::black);
    gb.tft.print(" delete old stuff");
    gb.save.del(SAVE_CLASSIC);
    gb.save.del(SAVE_META);
    gb.save.config(savefileDefaults);
    g_gameSettings = GameSettings();
    gb.save.set(2, 1);
    gb.save.set(SAVE_SETTINGS, g_gameSettings);
    gb.save.set(SAVE_SNAKE2, g_snake2Highscore);
  }
  gb.tft.setCursor(0, 50);
  gb.tft.setColor(Color::red, Color::black);
  gb.tft.print(" setup done!");
}


/// loop function
void loop() {
  menuLoop();
}


void menuLoop()
{
  gb.display.setFont(font5x7);
  Image menuImg(Snake5110metaMenuMain);
  uint8_t menuXPos = 0;
  uint8_t menuYPos = 0;
  uint8_t menuYCoord = 0;
  while(true)
  {
    if(!gb.update())
      continue;
    gb.display.clear();
    gb.display.setColor(BLACK, g_gameSettings.bgColor);
    menuYCoord = menuYPos == 0 ? 0 : menuYPos == 1 ? 8 : 16;
    gb.display.drawImage(0, 0, menuImg, 0, menuYCoord, 80, 64);

    if (gb.buttons.repeat(BUTTON_UP, 4) && menuYPos > 0)
      menuYPos--;
    if (gb.buttons.repeat(BUTTON_DOWN, 4) && menuYPos < 2)
      menuYPos++;
    if (gb.buttons.repeat(BUTTON_LEFT, 4) && menuXPos > 0)
      menuXPos--;
    if (gb.buttons.repeat(BUTTON_RIGHT, 4) && menuXPos < 1)
      menuXPos++;

    setMenuCursor(menuYCoord, menuXPos, menuYPos);

    if (!gb.buttons.pressed(BUTTON_A) && !gb.buttons.pressed(BUTTON_B))
      continue;
    if (menuXPos == 0 && menuYPos == 0) // Start Meta Game
    {
      g_gameMode.mode = GameMode::Meta;
      initGame();
      gameLoop();
    }
    else if (menuXPos == 1 && menuYPos == 0) // Use the classic start
    {
      g_gameMode.mode = GameMode::Classic;
      initGame();
      gameLoop();
    }
    else if (menuXPos == 0 && menuYPos == 1) // Wall less game mode
    {
      g_gameMode.mode = GameMode::Wallless;
      initGame();
      gameLoop();
    }
    else if (menuXPos == 1 && menuYPos == 1) // Set Game difficulty
      setDifficulty();
    else if (menuXPos == 0 && menuYPos == 2) // Show Highscore
    {
      g_classicHighscore.drawHighScores();
      g_metaHighscore.drawHighScores();
      g_snake2Highscore.drawHighScores();
    }
    else if (menuXPos == 1 && menuYPos == 2) // Show the credits
      showCredits();
  }
}


// basically a rect around current menu selection, topYpos is 0 or the y-shifted value of the menu image
void setMenuCursor(uint8_t topYpos, uint8_t xSelect, uint8_t ySelect) {
  xSelect = /*ySelect == 2 ||*/ xSelect == 0 ? 1 : 40;
  uint8_t height = /*2ySelect ==  ? 9 :*/ 25;
  uint8_t width = /*2ySelect == ? 77 :*/ 38;
  ySelect = ySelect == 0 ? 0 : ySelect == 1 ? 27 : 54;
  ySelect -= topYpos;
  gb.display.setColor(WHITE);
  gb.display.drawRect(xSelect, ySelect,width, height);
}


/// returns tru if a button was held long enough for triggering boost mode.
bool checkSpeedUp()
{
  if (gb.buttons.timeHeld(BUTTON_UP) != 0xFFF && gb.buttons.timeHeld(BUTTON_UP) > 5)
    return true;
  if (gb.buttons.timeHeld(BUTTON_LEFT) != 0xFFF && gb.buttons.timeHeld(BUTTON_LEFT) > 5)
    return true;
  if (gb.buttons.timeHeld(BUTTON_DOWN) != 0xFFF && gb.buttons.timeHeld(BUTTON_DOWN) > 5)
    return true;
  if (gb.buttons.timeHeld(BUTTON_RIGHT) != 0xFFF &&gb.buttons.timeHeld(BUTTON_RIGHT) > 5)
    return true;
  return false;
}

/// game loop function
void gameLoop() {
  bool isClassicMode = g_gameMode.mode == GameMode::Classic;
  while(true)
  {
    if(!gb.update()) { continue; }
    if(gb.buttons.pressed(BUTTON_MENU)) { return; }
    if(wallCollision()) { gameOver(true); return; }
    if(snakeBite()) { gameOver(false); return; }
    gb.display.clear();

    bool boostOn = g_gameSettings.allowSpeedUp && checkSpeedUp();
    if (c_debug) {
      gb.display.cursorX = 3;
      gb.display.cursorY = 10;
      gb.display.setColor(BLACK);
      gb.display.println(Coordinate::g_snakePos.x());
      gb.display.cursorX = 3;
      gb.display.println(Coordinate::g_snakePos.y());
    }

    g_delayCounter++;
    bool timeChanged = false;

    if ( boostOn || (g_delayCounter % g_levelModuloHelper) == 0)
      timeChanged = true;
    drawArena();
    gb.display.setColor(g_gameSettings.bgColor);

    auto oldButtonPress = g_lastButtonPressed; /// needed for speedup only
    /// for new mode: ignore button presses where snake would turn backwards, old mode: die at that point
    if (gb.buttons.repeat(BUTTON_UP, 1))
    { g_lastButtonPressed = isClassicMode ? UP : (g_lastTimeButtonPressed == DOWN) ? g_lastTimeButtonPressed : UP; }
    else if (gb.buttons.repeat(BUTTON_RIGHT,1))
    {g_lastButtonPressed = isClassicMode ? RIGHT : (g_lastTimeButtonPressed == LEFT) ? g_lastTimeButtonPressed : RIGHT; }
    else if (gb.buttons.repeat(BUTTON_DOWN, 1))
    {g_lastButtonPressed = isClassicMode ? DOWN : (g_lastTimeButtonPressed == UP) ? g_lastTimeButtonPressed : DOWN; }
    else if (gb.buttons.repeat(BUTTON_LEFT, 1))
    { g_lastButtonPressed = isClassicMode ? LEFT : (g_lastTimeButtonPressed == RIGHT) ? g_lastTimeButtonPressed : LEFT; }
    else if (gb.buttons.repeat(BUTTON_B, 1) || gb.buttons.repeat(BUTTON_A, 2))
    {
      g_lastButtonPressed = PAUSE;
      if (g_lastTimeButtonPressed != g_lastButtonPressed)
        gb.gui.popup("Pause",9);
    }
    if (timeChanged)
      g_lastTimeButtonPressed = g_lastButtonPressed;

    drawSnake(g_lastButtonPressed, timeChanged, boostOn);
    if (g_lastButtonPressed != PAUSE && timeChanged && snakeHasPrey(boostOn))
        { setNewNomPos(); }
    drawPrey();
  }
}


/// set difficulty:
void setDifficulty() {
  Image settingsImg(settingsMenu);
  Coordinate::g_snakePos.set2Position(12, 10);
  g_snake.reset();
  g_snake.addCoordinate(Coordinate::g_snakePos);
  g_snake.addCoordinate(Coordinate(11,10));
  g_snake.addCoordinate(Coordinate(10,10));
  g_delayCounter = 0;
  gb.display.setFont(font5x7);
  auto settingsPos = 0;
  auto bgColorPos = g_gameSettings.bgColorType;
  auto wallColorPos = g_gameSettings.wallColorType;
  uint8_t menuYCoord = 0, // used to move all the stuff around
      yPosHelper = -20, // used to allow central setting for text position
      lineY = 8; // used to define line height
  while(1){
    if(!gb.update())
      continue;
    gb.display.clear();

    if(gb.buttons.pressed(BUTTON_A))
    {
        gb.save.set( SAVE_SETTINGS , g_gameSettings );
        gb.gui.popup("Settings saved",3);
        return;
    }
    else if (gb.buttons.pressed(BUTTON_MENU) || gb.buttons.pressed(BUTTON_B))
        return;

    /// handle settings
    if (settingsPos > 0 && gb.buttons.repeat(BUTTON_UP, 2))
    {
      settingsPos--;
      // if (settingsPos > 1) g_snake.translate(UP);
    }
    if (settingsPos < 5 && gb.buttons.repeat(BUTTON_DOWN, 2))
    {
      settingsPos++;
      // if (settingsPos > 1) g_snake.translate(DOWN);
    }
    if (settingsPos == 0 && g_gameSettings.level > 1 && gb.buttons.repeat(BUTTON_LEFT, 2))
        g_gameSettings.level--;
    else if (settingsPos == 0 && g_gameSettings.level < c_maxLevel && gb.buttons.repeat(BUTTON_RIGHT, 2))
        g_gameSettings.level++;
    else if (settingsPos == 1 && g_gameSettings.snakeStyle && gb.buttons.repeat(BUTTON_RIGHT, 2))
        g_gameSettings.snakeStyle = false;
    else if (settingsPos == 1 && !g_gameSettings.snakeStyle && gb.buttons.repeat(BUTTON_LEFT, 2))
        g_gameSettings.snakeStyle = true;
    else if (settingsPos == 2 && bgColorPos < 2 && gb.buttons.repeat(BUTTON_RIGHT, 2))
        bgColorPos++;
    else if (settingsPos == 2 && bgColorPos > 0 && gb.buttons.repeat(BUTTON_LEFT, 2))
        bgColorPos--;
    else if (settingsPos == 3 && wallColorPos < 1 && gb.buttons.repeat(BUTTON_RIGHT, 2))
        wallColorPos++;
    else if (settingsPos == 3 && wallColorPos > 0 && gb.buttons.repeat(BUTTON_LEFT, 2))
        wallColorPos--;
    else if (settingsPos == 4 && g_gameSettings.allowSpeedUp && gb.buttons.repeat(BUTTON_RIGHT, 2))
        g_gameSettings.allowSpeedUp = false;
    else if (settingsPos == 4 && !g_gameSettings.allowSpeedUp && gb.buttons.repeat(BUTTON_LEFT, 2))
        g_gameSettings.allowSpeedUp = true;
    else if (settingsPos == 5 && g_gameSettings.alwaysShowScore && gb.buttons.repeat(BUTTON_RIGHT, 2))
        g_gameSettings.alwaysShowScore = false;
    else if (settingsPos == 5 && !g_gameSettings.alwaysShowScore && gb.buttons.repeat(BUTTON_LEFT, 2))
        g_gameSettings.alwaysShowScore = true;
    g_levelModuloHelper = c_maxLevel + 1 - g_gameSettings.level;
    g_gameSettings.setBgColorByType(bgColorPos);
    g_gameSettings.setWallColorByType(wallColorPos);
    menuYCoord = settingsPos < 2 ? 0 : 4*(settingsPos-1);
    gb.display.drawImage(0, 0, settingsImg, 0, menuYCoord, 80, 64);

    gb.display.setColor(WHITE);
    if (settingsPos == 0)
      gb.display.drawRect(1, 1, 36, 8);
    else if (settingsPos == 1)
      gb.display.drawRect(1, 41, 41, 8);
    else if (settingsPos == 2)
      gb.display.drawRect(1, 44, 30, 8);
    else if (settingsPos == 3)
      gb.display.drawRect(1, 47, 38, 8);
    else if (settingsPos == 4)
      gb.display.drawRect(1, 50, 52, 8);
    else if (settingsPos == 5)
      gb.display.drawRect(1, 53, 40, 8);
    gb.display.setColor(BLACK);

    uint8_t xPos = 14;
    uint8_t yPos = 37-menuYCoord;
    for(uint8_t i = 0; i < c_maxLevel ; i++)
    { // difficulty part
      uint8_t currentHeight= (i+1)*4 -1;

      if (i < g_gameSettings.level)
      { gb.display.fillRect(xPos, yPos,3,-currentHeight); }

      xPos += 7;
      // yPos -= 4;
    }

    /// snake animation part:
    g_delayCounter++;
    bool timeChanged = false;
    if ((g_delayCounter % g_levelModuloHelper) == 0)
      timeChanged = true;
    if (Coordinate::g_snakePos.x() > 17)
      Coordinate::g_snakePos.set2Position(10, 10 - (menuYCoord/4));
    drawSnake(RIGHT, timeChanged, false);

    /// selection part:
    auto selXpos = 0, selYpos = 0, selWidth = 7, selHeight = 6;
    if (settingsPos == 1 )
    {
      /// TODO: snake style here!
    }
    else if (settingsPos == 2 ) // bg color
    {
      selXpos = 35; selYpos = 45;
      if (bgColorPos == 1)
        selXpos = 51;
      else if (bgColorPos == 2)
        selXpos = 67;
    }
    else if (settingsPos == 3) // wall color
    {
      selXpos = 43; selYpos = 47;
      if (wallColorPos == 1)
        selXpos = 59;
    }
    else if (settingsPos == 4) // speed up
    {
      selXpos = 68; selYpos = 51; selWidth = 10;
      if (g_gameSettings.allowSpeedUp)
        { selXpos = 54; selWidth = 13; }
    }
    else if (settingsPos == 5) // show score
    {
      selXpos = 57; selYpos = 54; selWidth = 10;
      if (g_gameSettings.alwaysShowScore)
        { selXpos = 42; selWidth = 13; }
    }
    if (settingsPos > 1)
    {
      gb.display.setColor(WHITE);
      gb.display.drawRect(selXpos, selYpos, selWidth, selHeight);
    }

    gb.display.setColor(ORANGE);
    gb.display.drawRect(0, 0, gb.display.width(), gb.display.height());
  }
}


void showCredits() {
  gb.display.clear();
  while (true) {
    if (!gb.update())
        continue;
    gb.display.clear();
    gb.display.setColor(g_gameSettings.bgColor);
    gb.display.fillRect(0, 0, gb.display.width(), gb.display.height());
    gb.display.setColor(ORANGE);
    gb.display.drawRect(0, 0, gb.display.width(), gb.display.height());
    gb.display.setColor(BLACK, g_gameSettings.bgColor);
    gb.display.cursorX = 10;
    gb.display.cursorY = 5;
    gb.display.println(F("Snake 5110"));
    gb.display.cursorX = 5;
    gb.display.println(F("META v1.2 Dec23"));
    gb.display.cursorX = 5;
    gb.display.println(F("CC-BY-SA 2018"));
    gb.display.cursorX = 5;
    gb.display.println(F("Lady Awesome"));
    gb.display.cursorX = 5;
    gb.display.println(F("MakerSquirrel"));
    gb.display.cursorX = 5;
    gb.display.println(F("R0d0t (Highscore)"));
    if (gb.buttons.pressed(BUTTON_A) || gb.buttons.pressed(BUTTON_B) || gb.buttons.pressed(BUTTON_MENU)) {
        gb.sound.playOK();
        break;
    }
  }
}


/// triggered whenever we want to start the game
void initGame() {
    gb.pickRandomSeed(); //pick a different random seed each time for games to be different
    gb.display.setFont(font3x5);
    if (g_gameMode.mode == GameMode::Classic || g_gameMode.mode == GameMode::Wallless) {
      Coordinate::g_snakePos = g_snake.initClassic();
      Coordinate::g_nomPos = Coordinate(c_rasterX/2, c_rasterY/2);
      g_lastButtonPressed = RIGHT;
      g_lastTimeButtonPressed = RIGHT;
    }
    else if (g_gameMode.mode == GameMode::Meta) {
      Coordinate::g_snakePos = g_snake.initMeta();
      g_lastButtonPressed = PAUSE;
      g_lastTimeButtonPressed = PAUSE;
      setNewNomPos();
      for(auto i = 0 ; i <= c_rasterX; i++)
        { g_xWallsRemoved[i] = false; }
      for(auto i = 0 ; i <= c_rasterY; i++)
        { g_yWallsRemoved[i] = false; }
      g_xWallsRemaining = c_rasterX+1;
      g_yWallsRemaining = c_rasterY+1;
    }
    g_growNom = 0;
    g_score = 0;
    g_levelModuloHelper = c_maxLevel + 1 - g_gameSettings.level;
    g_delayCounter = 0;
}


void drawArena() {
  gb.display.setColor(g_gameSettings.bgColor);
  gb.display.fillRect(0, 0, gb.display.width(), gb.display.height());
  gb.lights.fill(g_gameSettings.bgColor);
  if (g_gameSettings.alwaysShowScore || g_lastButtonPressed == PAUSE)
  {
    gb.display.cursorX = 2;
    gb.display.cursorY = 2;
    gb.display.setColor(GRAY);
    gb.display.print(g_score);
  }
  if (g_gameMode.mode == GameMode::Wallless)
    return;
  gb.display.setColor(g_gameSettings.wallColor);
  gb.display.drawRect(0, 0, gb.display.width(), gb.display.height());
  if (g_gameMode.mode == GameMode::Classic)
    return;
  /// remove torn down wall parts
  gb.display.setColor(g_gameSettings.bgColor);
  if (g_xWallsRemoved[0])
  {
    gb.display.fillRect(0, 0, 5, 2);
    gb.display.fillRect(0, gb.display.height()-1, 5, 2);
  }
  if (g_yWallsRemoved[0])
  {
    gb.display.fillRect(0, 0, 2, 5);
    gb.display.fillRect(gb.display.width()-1, 0, 2, 5);
  }
  for(uint8_t i = 1; i < c_rasterX; i++)
  {
    if (!g_xWallsRemoved[i]) continue;
    gb.display.fillRect(i*4+1, 0, 5, 2);
    gb.display.fillRect(i*4+1, gb.display.height()-1, 5, 2);
  }
  for(uint8_t i = 1; i < c_rasterY; i++)
  {
    if (!g_yWallsRemoved[i]) continue;
    gb.display.fillRect(0, i*4+1, 2, 5);
    gb.display.fillRect(gb.display.width()-1, i*4+1, 2, 5);
  }
  gb.display.setColor(BLACK, g_gameSettings.bgColor);
  if (!c_debug)
    return;
  gb.display.cursorX = 1;
  gb.display.cursorY = 33;
  for(uint8_t i = 0; i < c_rasterX; i++)
  {
    if (!g_xWallsRemoved[i]) continue;
    gb.display.print(i);
  }
  gb.display.cursorX = 1;
  gb.display.cursorY = 39;
  for(uint8_t i = 0; i < c_rasterY; i++)
  {
    if (!g_yWallsRemoved[i]) continue;
    gb.display.print(i);
  }
}


void drawSnake(int8_t direction, bool timeChanged, bool boostMode) {
  gb.display.setColor(g_gameSettings.bgColor);
  if (g_growNom > 0)
  {
    uint ledColumn = Coordinate::g_snakePos.x() > c_rasterX/2 ? 1 : 0;
    int8_t y = Coordinate::g_snakePos.y();
    uint ledRow = y < c_rasterY/4 ? 0 : y < c_rasterY/2 ? 1 : 3*c_rasterY/4 ? 2 : 3;
    gb.lights.drawPixel(ledColumn, ledRow, ORANGE);
  }
  if (timeChanged)
  {
    bool hasMoved = Coordinate::g_snakePos.move(direction, g_gameMode.mode == GameMode::Classic, isOutOfBounds(Coordinate::g_snakePos), Coordinate::g_snakePos.isInArena());
    if (hasMoved)
    {
      if (g_growNom > 0)
      {
        g_snake.addCoordinate(Coordinate()); // dummy coordinate, is updated in moveCoordinates
        g_growNom -= 1;
      }
      g_snake.moveCoordinates(Coordinate::g_snakePos);
      if (c_debug)
      {
//                 g_cpuLoad = gb.getCpuLoad();
//                 g_ramUsage = gb.getFreeRam();
      }
    }
  }

  if (c_debug)
  {
    gb.display.cursorX = 10;
    gb.display.cursorY = 3;
//         gb.display.print(F("CPU/RAM: "));
//         // this itself does actually cost a lot of performance!
//         gb.display.print(g_cpuLoad);
//         gb.display.print(F("/"));
    gb.display.print(g_ramUsage);
  }
  uint8_t size = g_snake.size();
  if (!boostMode)
    gb.display.setColor(BLACK, g_gameSettings.bgColor);
  else
    gb.display.setColor(WHITE, g_gameSettings.bgColor);
  for (int index = 0; index < size; index++) /// draw step
  {
    const Coordinate& coord = g_snake.get(index);
    int8_t xPixel = coord.xPixel();
    int8_t yPixel = coord.yPixel();
    gb.display.drawFastHLine(xPixel, yPixel, 3);
    gb.display.drawFastHLine(xPixel, yPixel+1, 3);
    gb.display.drawFastHLine(xPixel, yPixel+2, 3);

    if (index > 0) /// polish step (fills gaps between the segments)
    {
      const Coordinate& otherCoord = g_snake.get(index-1);
      int8_t deltaX = coord.x() - otherCoord.x();
      int8_t deltaY = coord.y() - otherCoord.y();
      if (abs(deltaX) < 2 && abs(deltaY) < 2) /// skip parts where snake passes through wall gaps
      {
        gb.display.drawFastHLine(xPixel-deltaX, yPixel-deltaY, 3);
        gb.display.drawFastHLine(xPixel-deltaX, yPixel-deltaY+1, 3);
        gb.display.drawFastHLine(xPixel-deltaX, yPixel-deltaY+2, 3);
      }
    }
  }
}


void drawPrey() {
  gb.display.setColor(BLACK, g_gameSettings.bgColor);
  gb.display.drawCircle(Coordinate::g_nomPos.xPixel()+1, Coordinate::g_nomPos.yPixel()+1, 1);
  if (g_gameMode.mode == GameMode::Classic)
    return;
  if (Coordinate::g_shrinkNomPos.isInArena())
    gb.display.drawFastHLine(Coordinate::g_shrinkNomPos.xPixel(), Coordinate::g_shrinkNomPos.yPixel()+1, 3);
  if (Coordinate::g_growNomPos.isInArena())
  {
    gb.display.drawFastHLine(Coordinate::g_growNomPos.xPixel(), Coordinate::g_growNomPos.yPixel()+1, 3);
    gb.display.drawFastVLine(Coordinate::g_growNomPos.xPixel()+1, Coordinate::g_growNomPos.yPixel(), 3);
  }
  if (g_gameMode.mode == GameMode::Wallless)
    return;
  if (Coordinate::g_wallNomPos.isInArena())
    gb.display.drawRect(Coordinate::g_wallNomPos.xPixel(), Coordinate::g_wallNomPos.yPixel(), 3, 3);
}


/// New nompos is set to a place where no snake segment is found
void setNewNomPos()
{
  Coordinate::g_nomPos.setOffBounds(); // needed for to not interfere with new random free pos.
  Coordinate::g_nomPos = g_snake.getRandomFreePos();
  if (g_gameMode.mode == GameMode::Classic)
    return;
  Coordinate::g_shrinkNomPos.setOffBounds();
  Coordinate::g_growNomPos.setOffBounds();
  Coordinate::g_wallNomPos.setOffBounds();
  if (g_snake.size() < 6)
    return;
  uint8_t rngVal = random(0, 38);
  if (rngVal == 4 || rngVal == 2 || rngVal == 3 || rngVal == 33)
    Coordinate::g_shrinkNomPos = g_snake.getRandomFreePos();
  else if (rngVal == 23 || rngVal == 5 || rngVal == 7 || rngVal == 29)
    Coordinate::g_growNomPos = g_snake.getRandomFreePos();
  else if (g_gameMode.mode != GameMode::Wallless && rngVal == 13 || rngVal == 37 || rngVal == 1 || rngVal == 13)
    Coordinate::g_wallNomPos = g_snake.getRandomFreePos();
}


/// returns true if any of the prey types have been eaten
bool snakeHasPrey(bool isBoostMode)
{
  bool nom = false;
  auto scoreUp = isBoostMode ? c_maxLevel : g_gameSettings.level;
  if (Coordinate::g_nomPos == Coordinate::g_snakePos)
  {
    g_growNom += 1; // will be count down until growing is complete (g_growNom == 0)
    g_score += scoreUp;
    nom = true;
    gb.sound.fx(nomFX);
  }
  else if (g_gameMode.mode != GameMode::Classic && Coordinate::g_shrinkNomPos == Coordinate::g_snakePos)
  {
    nom = true;
    g_snake.shrink(4);
    g_score-=4;
    gb.sound.fx(shrinkNomFX);
  }
  else if (g_gameMode.mode != GameMode::Classic && Coordinate::g_growNomPos == Coordinate::g_snakePos)
  {
    nom = true;
    g_growNom += 3; // will be count down to zero until growing is complete
    g_score+= 3*scoreUp;
    gb.sound.fx(growNomFX);
  }
  else if (g_gameMode.mode == GameMode::Meta && Coordinate::g_wallNomPos == Coordinate::g_snakePos)
  {
    nom = true;
    g_score += scoreUp;
    deleteRandomWallElement();
    gb.sound.fx(wallNomFX);
  }
  return nom ? true : false;
}


/// checks if snake runs into a wall
bool wallCollision()
{ return isOutOfBounds(Coordinate::g_snakePos); }


void gameOver(bool wallCrash)
{
  gb.display.setColor(g_gameSettings.bgColor);
  if (wallCrash)
    gb.gui.popup("Wallcrash!",9);
  else
    gb.gui.popup("Snakebite!",9);

  auto& currentHighScore = g_gameMode.mode == GameMode::Classic ? g_classicHighscore :
                              (g_gameMode.mode == GameMode::Meta ? g_metaHighscore : g_snake2Highscore);
  auto saveSlot = g_gameMode.mode == GameMode::Classic ? SAVE_CLASSIC :
                              (g_gameMode.mode == GameMode::Meta ? SAVE_META : SAVE_SNAKE2);

  bool newHighScore = currentHighScore.checkHighScore(g_score);
  if (newHighScore)
    gb.sound.fx(highscoreFX);
  else
    gb.sound.fx(gameoverFX);
  currentHighScore.showScore(g_score);
  if (newHighScore)
  {
    currentHighScore.updateHighscore(g_score);
    gb.save.set( saveSlot , currentHighScore );
  }
  currentHighScore.drawHighScores();
}


/// checks if snake bites itself
bool snakeBite()
{ return g_snake.isPartOfSnake(Coordinate::g_snakePos); }
