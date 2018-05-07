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
      _tempPos = 0;
      _pos = 3;
      _alive = true;
    }
    void Die(CRGB leds[]) {
      if (_color == CRGB(102,51,153)) {

      } else {
        for (int i = 0; i < 10; i++) {
          leds[_pos + i] = CRGB(0, random(255), random(255));
          leds[_pos - i] = CRGB(0, random(255), random(255));
          FastLED.show();
        }
        FastLED.clear();
        _pos = 3;
      }
    }
};

#endif