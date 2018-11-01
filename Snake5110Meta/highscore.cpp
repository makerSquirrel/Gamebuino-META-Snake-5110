#include "highscore.h"


uint16_t HighScore::getCurrentOffset()
{
  static uint16_t s_offsetCounter = 0; // local static variable to guarantee correct counting over all Highscore instances
  uint16_t currentOffset = s_offsetCounter;
    s_offsetCounter += HIGHSCORE_COUNT * (NAME_LETTERS+3);
    return currentOffset;
}


bool HighScore::showScore(uint16_t score)
{
  bool isHighScore = checkHighScore(score);
    while (true) {
        if (!gb.update())
            continue;
        gb.display.clear();
        gb.display.setColor(m_bgColor);
        gb.display.fillRect(0, 0, gb.display.width(), gb.display.height());
        gb.display.setColor(m_textColor);
        gb.display.cursorY = 3;
        gb.display.cursorX = 14;
        gb.display.println(F("GAME OVER!"));
        gb.display.println(F(""));
        gb.display.cursorX = 12;
        gb.display.println(F("YOUR SCORE:"));
        gb.display.cursorX = 30;
        gb.display.println(score);
        gb.display.cursorX = 0;
        if (isHighScore)
            gb.display.println(F("NEW HIGHSCORE!"));
        if (gb.buttons.pressed(BUTTON_A) || gb.buttons.pressed(BUTTON_B) || gb.buttons.pressed(BUTTON_MENU))
            { break; }
    }
    return isHighScore;
}


void HighScore::updateHighscore(uint16_t newHighScore)
{
  char playerName[NAME_LETTERS];
  gb.getDefaultName(playerName);
  gb.gui.keyboard("New Highscore!",playerName);
  strcpy(m_highScoreNames[HIGHSCORE_COUNT - 1],playerName);
//   m_highScoreNames[HIGHSCORE_COUNT - 1] = playerName;
//     gb.getDefaultName(m_highScoreNames[HIGHSCORE_COUNT - 1]);
//     gb.gui.keyboard("New Highscore!",m_highScoreNames[HIGHSCORE_COUNT - 1]);
    m_highScores[HIGHSCORE_COUNT - 1] = newHighScore;

    //Sort highscores
    for (byte i = HIGHSCORE_COUNT - 1; i > 0; i--) {
        if (m_highScores[i - 1] >= m_highScores[i])
            { break; }
        char tempName[NAME_LETTERS];
        strcpy(tempName, m_highScoreNames[i - 1]);
        strcpy(m_highScoreNames[i - 1], m_highScoreNames[i]);
        strcpy(m_highScoreNames[i], tempName);
        uint16_t tempScore;
        tempScore = m_highScores[i - 1];
        m_highScores[i - 1] = m_highScores[i];
        m_highScores[i] = tempScore;

    }
    //update minimum highscore
    m_minHighScore = m_highScores[HIGHSCORE_COUNT-1];
}


void HighScore::drawHighScores() {
  gb.display.clear();
  while (true) {
    if (!gb.update())
        continue;
    gb.display.clear();
    gb.display.setColor(m_bgColor);
    gb.display.fillRect(0, 0, gb.display.width(), gb.display.height());
    gb.display.setColor(m_textColor);

    //Title
    gb.display.cursorX = 24;
    gb.display.cursorY = 3;
    if (m_nameIsSet)
      gb.display.println(m_name);
    gb.display.cursorX = 18;
    gb.display.println(F("HIGH SCORES"));

    gb.display.cursorY = gb.display.fontHeight * 3;
    for (byte i = 0; i < HIGHSCORE_COUNT; i++) {
        gb.display.cursorX = 6;

        //Name
        if (m_highScores[i] == 0)
            { gb.display.print('-'); }
        else
            { gb.display.print(m_highScoreNames[i]); }

        //Score
        if (m_highScores[i] > 9999) {
          gb.display.cursorX = gb.display.width() - 6 - 5 * gb.display.getFontWidth();
        } else if (m_highScores[i] > 999) {
          gb.display.cursorX = gb.display.width() - 6 - 4 * gb.display.getFontWidth();
        } else if (m_highScores[i] > 99) {
          gb.display.cursorX = gb.display.width() - 6 - 3 * gb.display.getFontWidth();
        } else if (m_highScores[i] > 9) {
          gb.display.cursorX = gb.display.width() - 6 - 2 * gb.display.getFontWidth();
        } else {
          gb.display.cursorX = gb.display.width() - 6 - gb.display.getFontWidth();
        }
        gb.display.cursorY = (gb.display.fontHeight * 3) + (gb.display.fontHeight * i);
        gb.display.println(m_highScores[i]);
    }

    if (gb.buttons.pressed(BUTTON_A) || gb.buttons.pressed(BUTTON_B) || gb.buttons.pressed(BUTTON_MENU)) {
        gb.sound.playOK();
        break;
    }
  }
}
