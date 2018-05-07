#ifndef Enemy_h
#define Enemy_h

#include "Arduino.h"
#include "FastLED.h"

class Enemy
{
  public:
    int _dir;
    int _pos;
    int _prevPos;
    bool _alive;
    int _speed;
    int _tempPos;

    Enemy(void) {
      _alive = false;
    }
    void Spawn(int pos, int dir, int speed_) {
      _pos = pos;
      _dir = dir;
      _alive = true;
      _speed = speed_;
    }
    void Kill(bool reason, CRGB leds[]) {
      _alive = false;
      if (reason) {
        for (int i = 0; i < 10; i++) {
          leds[_pos + i] = CRGB(random(255), 0, 0);
          leds[_pos - i] = CRGB(random(255), 0, 0);
          FastLED.show();
        }
        FastLED.clear();
      }
    }
};

#endif