#ifndef Morse_h
#define Morse_h

#include "Arduino.h"
#include "FastLED.h"

class Character
{
  public:
    //variables
    int _pos;
    int _prevPos;
    bool _alive;
    int _dir;
    bool _shield;
    bool _jump;
    bool _attack;
    CRGB _color;
    int _tempPos;
    long _attackTime;
    long _jumpTime;
    long _shieldTime;

    Character() {
      _jump = false;
      _shield = false;
      _attack = false;
      _color = CRGB::Red;
      _tempPos = 0;
      _pos = 3;
      _alive = true;
    }
};

#endif