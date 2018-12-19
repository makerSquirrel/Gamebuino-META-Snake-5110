#pragma once
#include <Arduino.h>
#include "globalVariables.h"
#include "coordinate.h"
/** ******************   SNAKE CLASS DECLARATION: */

struct Snake
{ // OPTIONAL  TODO: implement safe array class allowing range based for loops: https://www.cprogramming.com/c++11/c++11-ranged-for-loop.html

    Snake() : m_snakeSize(0) {}

    void addCoordinate(const Coordinate& newCoord)
    {
      if (!newCoord.isInArena() || m_snakeSize >= g_arenaSize) return;
      m_snakeCoordinates[m_snakeSize] = newCoord;
      m_snakeSize++;
    }

    /// used for classic and wallless, returns coordinate of head.
    Coordinate initClassic();
    Coordinate initMeta();

    void shrink(uint8_t shrinkVal = 3) { m_snakeSize -= shrinkVal; }

    void reset()
    {
      for(int8_t index = 0; index < m_snakeSize ; index++)
        { m_snakeCoordinates[index] = Coordinate(); }
      m_snakeSize = 0;
    }

    /// == contains
    bool isPartOfSnake(const Coordinate& checkPos, bool checkHead = false) const
    {
      int8_t index = 0;
      if (!checkHead)
        index = 1;
      for(; index < m_snakeSize; index++)
      {
        if (checkPos == m_snakeCoordinates[index])
          return true;
      }
      return false;
    }

    void moveCoordinates(const Coordinate& newHead)
    {
      for(int8_t index = m_snakeSize; index > 0 ; index--)
        { m_snakeCoordinates[index] = m_snakeCoordinates[index-1]; }
      m_snakeCoordinates[0] = newHead;
    }

    // moves all coordinates of the snake one up
    void translate(int8_t direction);

    /// sets the coordinate to a new random position that is not in use.
    Coordinate getRandomFreePos() const;

    /// TODO: safety check!
    const Coordinate& get(uint8_t index) const { return m_snakeCoordinates[index]; }

    uint8_t size() const { return m_snakeSize; }

    Coordinate m_snakeCoordinates[g_arenaSize];
    uint8_t m_snakeSize;
};
