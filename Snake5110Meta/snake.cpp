#include "snake.h"
#include "coordinate.h"


Coordinate Snake::initClassic()
{
  reset();
  addCoordinate(Coordinate(8,c_rasterY-1));
  addCoordinate(Coordinate(7,c_rasterY-1));
  addCoordinate(Coordinate(6,c_rasterY-1));
  addCoordinate(Coordinate(5,c_rasterY-1));
  addCoordinate(Coordinate(4,c_rasterY-1));
  addCoordinate(Coordinate(3,c_rasterY-1));
  addCoordinate(Coordinate(2,c_rasterY-1));
  addCoordinate(Coordinate(1,c_rasterY-1));
  addCoordinate(Coordinate(0,c_rasterY-1));
  return m_snakeCoordinates[0];
}


Coordinate Snake::initMeta()
{
  reset();
  addCoordinate(Coordinate(c_rasterX/2, c_rasterY/2));
  return m_snakeCoordinates[0];
}


Coordinate Snake::getRandomFreePos() const
{
  Coordinate newCoord;
  bool inUse = true;
  while(inUse)
  {
    newCoord = Coordinate(random(0, c_rasterX), random(0, c_rasterY));
    inUse = isPartOfSnake(newCoord, true);
    if (!inUse && Coordinate::g_nomPos.isInArena())
        inUse = ((newCoord == Coordinate::g_nomPos) ? true : false);
    if (!inUse && Coordinate::g_growNomPos.isInArena())
        inUse = ((newCoord == Coordinate::g_growNomPos) ? true : false);
    if (!inUse && Coordinate::g_shrinkNomPos.isInArena())
        inUse = ((newCoord == Coordinate::g_shrinkNomPos) ? true : false);
    if (!inUse && Coordinate::g_wallNomPos.isInArena())
        inUse = ((newCoord == Coordinate::g_wallNomPos) ? true : false);
  }
  return newCoord;
}

void Snake::translate(int8_t direction)
{
  for(int8_t index = 0; index < m_snakeSize ; index++)
    { m_snakeCoordinates[index].move(direction, true, false, false); }
}
