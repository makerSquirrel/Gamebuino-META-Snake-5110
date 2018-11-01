# Gamebuino-META-Snake-5110
A Gamebuino META version (https://gamebuino.com/)  of the Snake 5110 game originally programmed for the Gamebuino Classic. If interested in a fully working Gamebuino Classic version (fully compatible with MAKERbuino, https://www.makerbuino.com/), check out: https://github.com/makerSquirrel/Gamebuino-Classic-Snake-5110 .

A Snake clone (made by Lady Awesome and MakerSquirrel, with modified highscore code found in a R0d0t game ;) ).
This version orients itself on Snake 1, which was running on the original Nokia 5110 (hence the name ;) )

Steering of the snake is done with the arrow keys, pausing the game can be done with A or B button, leaving the game with C button.
There are two game modes:

- In the standard mode the short snake only starts moving after clicking a direction and when steering backwards the snake does not die. Next to the normal stuff to eat for the snake, there are several new types:
  - "GrowNoms" increase snake size by several bits, but bring more points,
  - "ShrinkNoms" reduce the snake size, but lowers score a bit and
  - "WallNoms": eating a piece removes a bit from the wall, which allows the snake passing through and entering at opposite side.
- The classic mode tries to mimic the original Snake version, where you start with the longer snake (which automatically starts when starting the game) and where steering backwards kills the Snake.

Since both versions are of different difficulty, they have different Highscores.  For both versions 9 different speed levels can be chosen, the higher the difficulty, the more points are gained when catching a dot with the snake.


Version history:

v0.1: Oct14-2018, so buggy that it does crash when entering the menu.
v0.8: Oct20-2018, mostly working, but no sound and some minor bugs exist.
v1.0: Nov01-2018, works. Please report bugs if you find any!
