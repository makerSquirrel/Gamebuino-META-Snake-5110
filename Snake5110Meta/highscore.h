#pragma once
#include <Arduino.h>
#include <Gamebuino-Meta.h>
#define HIGHSCORE_COUNT 5
#define NAME_LETTERS 13
/*template<typename HighscoreType>*/ class HighScore
{
private:

uint16_t m_ROMOffset;
uint16_t m_minHighScore;
uint16_t m_highScores[HIGHSCORE_COUNT];
char m_highScoreNames[HIGHSCORE_COUNT][NAME_LETTERS];
Color m_textColor;
Color m_bgColor;
char m_name[NAME_LETTERS]; // name of the highscore (only relevant if you want to use multiple separate scores in the same game)
bool m_nameIsSet;

/// returns correct offset for reading/writing in EEPROM, to be called for each instance created (in constructor)
static uint16_t getCurrentOffset();

public:
  HighScore(char* name = nullptr, Color textColor = WHITE, Color bgColor = BLACK) : m_minHighScore(0), m_ROMOffset(HighScore::getCurrentOffset()), m_textColor(textColor), m_bgColor(bgColor), m_nameIsSet(false)
  { setName(name); }
  
  void setTextColor(Color& newCol) { m_textColor = newCol; }
  void setBgColor(Color& newCol) { m_bgColor = newCol; }
  void setName(char* name)
  {
    if (!name)
      return;
    strcpy(m_name,name);
    m_nameIsSet = true;
  }
    
/// returns true if given value is a new highscore:
bool showScore(uint16_t score);

void updateHighscore(uint16_t newHighScore); // writes highscore to SD Card, should not be called more often than needed (i.e. only if showScore returns true)

void drawHighScores(); // prints highscores to screen, but only valid entries if loadHighscores() was called before at least once.
};
