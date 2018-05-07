#ifndef Boss_h
#define Boss_h

#include "Arduino.h"
#include "Enemy.h"

class Boss : public Enemy
{
  public:
    int _teleport;
    int _lives = 5;
};

#endif