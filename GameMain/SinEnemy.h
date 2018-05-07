#ifndef SinEnemy_h
#define SinEnemy_h

#include "Arduino.h"
#include "Enemy.h"

class SinEnemy : public Enemy
{
  public:
    int _upper;
    int _lower;

    void Spawn(int pos, int upper, int lower, int speed_) {
      _upper = upper;
      _lower = lower;
      _dir = 1;
      _pos = pos;
      _alive = true;
      _speed = speed_;
    }
};

#endif