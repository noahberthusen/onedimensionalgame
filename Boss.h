#ifndef Boss_h
#define Boss_h

#include "Arduino.h"
#include "Enemy.h"

class Boss : public Enemy
{
  public:
    int guck[50];
    int _teleport;
    int _guckCount = 0;
    int _lives = 4;
};

#endif