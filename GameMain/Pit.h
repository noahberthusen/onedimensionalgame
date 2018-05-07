#ifndef Pit_h
#define Pit_h

#include "Arduino.h"

class Pit
{
  public:
    bool _alive;
    int _tempPos;
    int _pos;
    int _size;

    Pit() {
      _alive = false;
    }
    void spawnPit(int pos, int size_) {
      _tempPos = 0;
      _size = size_;
      _alive = true;
      _pos = pos;
    }
    void Kill() {
      _alive = false;
    }
};

#endif