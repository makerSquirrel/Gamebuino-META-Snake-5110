#ifndef SOUNDFX_H
#define SOUNDFX_H
#include <Gamebuino-Meta.h>


/** https://gamebuino.com/creations/sound-fx-and-patterns
 * 
 * Type: Either SQUARE or NOISE, determines the waveform
 * Continue_flag: Set to 0 if this i*s the last sound_FX in the sound. Otherwise 1.
 * Volume start: Volume (amplitude) at the start of the sound_FX
 * Volume sweep: Volume (amplitude) variation over time (negative values dim volume over time, positive values amplify)
 * Period sweep: Period variation over time (negative values shrink the period over time, positive values extend)
 * Period start: Period at the start of the sound_FX
 * Length: Duration of the sound_FX in 20 ms steps, i.e. length 3 = 60ms
 * 

 const Gamebuino_Meta::Sound_FX mySfx[] = {
 {uint8_t Type, uint8_t Continue_flag, uint8_t Volume start, int8_t Volume sweep, 
 int8_t Period Sweep, uint8_t Period Start, uint8_t Length},  // 1st sound
 {...},  // 2nd sound
 };
 
 
 
 * 
 * */

/// nomFX
const Gamebuino_Meta::Sound_FX nomFX[] = {
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,0,100,-51,0,13,1}
};


/// growNomFX
const Gamebuino_Meta::Sound_FX growNomFX[] = {
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,1,100,0,-8,22,3},
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,0,100,-51,0,13,1}
};


/// shrinkNomFX
const Gamebuino_Meta::Sound_FX shrinkNomFX[] = {
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,1,100,-51,0,13,1},
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,0,100,0,12,16,3}
};


/// wallNomFX
const Gamebuino_Meta::Sound_FX wallNomFX[] = {
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,1,100,0,0,42,3},
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,0,100,-51,0,13,1}
};


/// gameoverFX
const Gamebuino_Meta::Sound_FX gameoverFX[] = {
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,0,100,0,0,400,8}
};


/// highscoreFX
const Gamebuino_Meta::Sound_FX highscoreFX[] = {
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,1,100,0,0,30,5},
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,1,0,0,0,30,1},
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,1,100,0,0,30,3},
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,1,0,0,0,30,1},
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,1,100,0,0,30,3},
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,1,0,0,0,30,1},
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,1,100,0,0,22,10},
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,1,0,0,0,30,1}
};
#endif // SOUNDFX_H
