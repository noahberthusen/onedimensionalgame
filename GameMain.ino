#include <FastLED.h>

// LED setup
#define NUM_LEDS 149
#define DATA_PIN 5
#define LED_TYPE WS2812B
#define BRIGHTNESS 100
#define COLOR_ORDER GRB

// Game
#define REFRESH_INTERVAL 1000
CRGB leds[NUM_LEDS];

int xPin = A1;
int yPin = A0;
int joyButtonPin = 2;

int xPosition = 0;
int yPosition = 0;
int joyButtonState = 0;

class Character
{
  private:
    
  public:
    //variables
    int _pos;
    int _prevPos;
    boolean _alive;
    int _dir;
    boolean _jump;
    boolean _attack;
    CRGB _color;
    long _tempPos;

    //constructor
    Character() {
      _jump = false;
      _color = CRGB::Red;
      _tempPos = 0;
      _pos = 0;
      _alive = true;
      //_ability = CRGB::Red;
    }
};

class Enemy
{
  public:
    int _dir;
    int _pos;
    int _prevPos;
    boolean _alive;
    int _speed;
    long _tempPos;
    
    Enemy(void) {
      _alive = false;
      _tempPos = 0;
      _pos = 0;
    }
    virtual void Spawn(int pos, int dir, int speed_) {
      _pos = pos;
      _dir = dir;
      _alive = true;
      _speed = speed_;
    }
    void Kill(boolean reason) {
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

class sinEnemy
{
  public:
    int _dir = 1;
    int _pos;
    int _upper;
    int _lower;
    int _prevPos;
    boolean _alive;
    int _speed;
    long _tempPos;
    
    sinEnemy(void) {
      _alive = false;
      _tempPos = 0;
      _pos = 0;
    }
    void Spawn(int pos, int upper, int lower, int speed_) {
      _upper = upper;
      _lower = lower;
      _pos = pos;
      _alive = true;
      _speed = speed_;
    }
    void Kill(void) {
      _alive = false;
    }
};

class Boss
{
  public:
    int guck[50];
    int _dir;
    int _pos;
    int _prevPos;
    boolean _alive;
    int _speed;
    long _tempPos;
    int _teleport;
    int _guckCount = 0;
    int _lives = 4;
    
    Boss(void) {
      _alive = false;
      _tempPos = 0;
      _pos = 0;
    }
    void Spawn(int pos, int dir, int speed_) {
      _pos = pos;
      _dir = dir;
      _alive = true;
      _speed = speed_;
    }
    void Kill(void) {
      _alive = false;
    }
};

class Pit
{
  private:
  public:
    boolean _alive;
    int _tempPos;
    int _pos; 
    int _size; 
    Pit(void) {
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

Character hero;
Boss boss;
// game objects
const int numEnemies = 3;
Enemy enemies[numEnemies] = 
{
  Enemy(), Enemy(), Enemy()
};
const int numSin = 3;
sinEnemy sinEnemies[numSin] = 
{
  sinEnemy(), sinEnemy(), sinEnemy()
};
const int numPits = 3;
Pit pits[numPits] = 
{
  Pit(), Pit(), Pit()
};

int level = 1;


void setup() {
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setDither(1);

  Serial.begin(9600);
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(joyButtonPin, INPUT_PULLUP); 

  setupLevel(level);
}

void loop () {
  long mm = millis();
  long lastRefreshTime = 0;
    if (mm - lastRefreshTime >= REFRESH_INTERVAL) {
      lastRefreshTime += REFRESH_INTERVAL;
      drawCharacter(hero);
      
      switch(level) {
        case 1:
          drawExit();
          break;
        case 2:
          drawExit();
          drawEnemy();
          break;
        case 3:
          drawExit();
          drawPit();
          break;    
        case 4:
          drawExit();
          drawPit(); 
          drawSinEnemy(); 
          drawEnemy();        
          break;
        case 5:
          drawExit();
          drawPit();
          drawEnemy();
          drawSinEnemy(); 
          break;
        case 6:
          drawExit();
          drawEnemy();
          drawSinEnemy();
          break;
        case 7:
          drawBoss(boss);
          break;
        default:
          break;     
      }
      FastLED.show();
    } 

}

void setupLevel(int level) {
  switch(level) {
    case 1:
      break;
    case 2:
      enemies[0].Spawn(130,-1,50);
      break;
    case 3:
      pits[0].spawnPit(50,3);
      pits[1].spawnPit(100,5);
      break;
    case 4:
      enemies[0].Spawn(130,-1,50);
      pits[1].spawnPit(110,3);
      sinEnemies[0].Spawn(80, 100, 50, 150);
      break;
    case 5:
      enemies[0].Spawn(40,1,100);
      sinEnemies[0].Spawn(70, 120, 60, 100);
      sinEnemies[1].Spawn(120, 130, 80, 80);
      pits[0].spawnPit(70,5);
    case 6:
      enemies[0].Spawn(40,1,150);
      enemies[1].Spawn(100,1,150);
      enemies[2].Spawn(140,1,150);
      sinEnemies[0].Spawn(20, 130, 20, 100);
    case 7:
      boss.Spawn(130, -1, 10);
    default:
      break;
  }
}

void getInput(Character &character) {
  yPosition = -map(analogRead(yPin), 0, 1023, -80, 80);

  //adjust for threshold values
  yPosition = (yPosition <= 3 && yPosition >= -3) ? 0 : yPosition;
  joyButtonState = digitalRead(joyButtonPin);
  xPosition = map(analogRead(xPin), 0, 1023, -80, 80);
  character._dir = yPosition < 0 ? -1 : 1;

  if (xPosition < -4 && (yPosition > -10 && yPosition < 10)) {
    character._color = CRGB::Yellow;
    if (!character._jump && character._pos < 138) {
      character._jump = true;
      leds[character._pos] = CRGB::Black;
      character._pos += (7 * character._dir);
      enemyInteraction();
    }
  } else if (xPosition > 0 && (yPosition > -10 && yPosition < 10)) {
    character._color = CRGB(102, 51, 153);
  } else {
    character._color = CRGB::White;
    character._jump = false; 
    if (character._attack) {
      if (joyButtonState == 1) {
        character._attack = false;
      } else {
        for (int i = 1; i < 4; i++) {
          leds[hero._pos + i] = CRGB::Black;
          leds[hero._pos - i] = CRGB::Black;
        }
      } 
    }
    if (!character._attack && joyButtonState == 0 && yPosition == 0) {
      character._attack = true;
      for (int i = 1; i < 4; i++) {
          leds[hero._pos + i] = CRGB::Salmon;
          leds[hero._pos - i] = CRGB::Salmon;
          //FastLED.show();
      }
    } 
  }
  leds[character._pos] = character._color;
}

void enemyInteraction() {
  for (int i = 0; i < numEnemies; i++) {
    if (enemies[i]._alive) {
      if (hero._pos == enemies[i]._pos && hero._pos > 0) {
        if (hero._color == CRGB(102,51,153)) {
          
        } else {
          Die();
        }
      }
      if (abs(hero._pos - enemies[i]._pos) <= 3 && hero._attack) {
        enemies[i].Kill(true);
      }
    }
  }
  for (int i = 0; i < numSin; i++) {
    if (sinEnemies[i]._alive) {
      if (hero._pos == sinEnemies[i]._pos && hero._pos > 0) {
        if (hero._color == CRGB(102,51,153)) {
          
        } else {
          Die();
        }
      }
    }
    if (abs(hero._pos - sinEnemies[i]._pos) <= 3 && hero._attack) {
      sinEnemies[i].Kill();
    }
  }
  for (int i = 0; i < numPits; i++) {
    if (pits[i]._alive) {
      for (int j = 0; j < pits[i]._size; j++) {
        if (hero._pos == pits[i]._pos+j){
          Die();
        }
      }
    }
  }
}
  //boss
  /*if (boss._alive) {
    if (hero._pos == boss._pos) {
        Die();
      }
    for (int i = 0; i < boss._lives; i++) {
      if (hero._pos == boss._pos+i) {
        Die();
      }
    }
    if (abs(boss._pos - hero._pos) <= 3 && hero._attack) {
      boss._lives -= 1;
      boss._speed += 10;
      boss._pos = random(10,130);
      if (boss._lives == 0) {
        boss.Kill();
        drawExit();
      }
    }
    if (level == 7) {
      for (int i = 0; i < boss._guckCount; i++) {
        if (hero._pos == boss.guck[i]) {
          Die();
        }
      }
    }
  }*/

void drawEnemy() {
  for (int i = 0; i < numEnemies; i++) {
    if (enemies[i]._alive) {
      enemies[i]._tempPos += enemies[i]._speed;
    
      if (enemies[i]._tempPos > 1000) {
        
        enemies[i]._prevPos = enemies[i]._pos;
        enemies[i]._pos += (1 * enemies[i]._dir);

        if (enemies[i]._pos < 0) {
          enemies[i]._prevPos = 0;
          enemies[i]._pos = NUM_LEDS-1;
        } else if (enemies[i]._pos > NUM_LEDS-1) {
          enemies[i]._prevPos = NUM_LEDS-1;
          enemies[i]._pos = 0;
        }
    
        leds[enemies[i]._prevPos] = CRGB::Black;
        leds[enemies[i]._pos] = CRGB::Red;
        
        
        enemies[i]._tempPos = 0; 
      }
      
    }
  }
}

void drawSinEnemy() {
  for (int i = 0; i < numSin; i++) {
    if (sinEnemies[i]._alive) {
      sinEnemies[i]._tempPos += sinEnemies[i]._speed;

      if (sinEnemies[i]._tempPos > 1000) {
       
        sinEnemies[i]._prevPos = sinEnemies[i]._pos;
        sinEnemies[i]._pos += (1 * sinEnemies[i]._dir);

        if (sinEnemies[i]._pos == sinEnemies[i]._upper || sinEnemies[i]._pos == sinEnemies[i]._lower) {
          sinEnemies[i]._dir *= -1;
        }
  
        leds[sinEnemies[i]._prevPos] = CRGB::Black;
        leds[sinEnemies[i]._pos] = CRGB::Red;
        sinEnemies[i]._tempPos = 0;
      }
    }
  }
}

void drawBoss(Boss &boss) {
  if (boss._alive) {
    boss._tempPos += boss._speed;
    if (boss._tempPos > 1000) {
      boss._prevPos = boss._pos;
      boss._pos += (1 * boss._dir);
      boss._tempPos = 0;
    }

    boss._teleport += 1;
    if (boss._teleport > 1000) {
      boss._pos = random(10,130);
      boss.guck[boss._guckCount] = boss._pos;
      boss._guckCount += 1;
      if (boss._guckCount > 10) boss._guckCount = 0;
      boss._dir *= -1;
      boss._teleport = 0;
    }

    //drawing boss
    for (int i = 0; i < boss._lives; i++) {
      leds[boss._prevPos + i] = CRGB::Black;
    }
    for (int i = 0; i < boss._lives; i++) {
      leds[boss._pos + i] = CRGB::Green;
    }
    //drawing guck
    for (int i = 0; i < boss._guckCount; i++) {
      leds[boss.guck[i]] = CRGB::Green;
    }
     
    if (boss._pos == 10) {
      boss._pos = random(30,130);
    } else if (boss._pos == NUM_LEDS) {
      boss._pos = random(30,130);
    }
  }
}

void drawCharacter (Character &character) {
  if (character._alive) {
    getInput(character);
    enemyInteraction();

    if (hero._pos >= NUM_LEDS-1) {
      hero._pos = 0;
      //FastLED.clear();
      Win();
    } else if (hero._pos < 0) {
      hero._pos = 0;
    }
    
    character._tempPos += abs(yPosition);
    if (character._tempPos > 1000) {
      
      character._prevPos = character._pos;
      character._pos += (1 * (yPosition < 0 ? -1 : 1));
  
      leds[character._prevPos] = CRGB::Black;
      leds[character._pos] = character._color;
      
      character._tempPos = 0; 
    } 
  }
}

void Die() {
  if (NUM_LEDS - hero._pos < 11 || hero._pos < 11) {
    hero._pos = 0;
    return;
  } else {
    for (int i = 0; i < 10; i++) {
      leds[hero._pos + i] = CRGB(0, random(255), random(255));
      leds[hero._pos - i] = CRGB(0, random(255), random(255));
      FastLED.show();
    }
    FastLED.clear();
    hero._pos = 0;
  }
}

void Win() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[NUM_LEDS-1-i] = CRGB::Green;
    FastLED.show();
    delay(5);
  }
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[NUM_LEDS-1-i] = CRGB::Black;
    FastLED.show();
    delay(5);
  }
  for (int i = 0; i < numEnemies; i++) {
    enemies[i].Kill(false);
  }
  for (int i = 0; i < numPits; i++) {
    pits[i].Kill();
  }
  for (int i = 0; i < numSin; i++) {
    sinEnemies[i].Kill();
  }
  level += 1;
  setupLevel(level);
  FastLED.clear();
}

void drawPit() {
  const int numColors = 6;
  CRGB colors[numColors] = 
  {
    CRGB::Red, CRGB::DarkRed, CRGB::Red, CRGB::DarkRed, CRGB::Red, CRGB::DarkRed
  };
  for (int i = 0; i < numPits; i++) {
    if (pits[i]._alive) {
      //refine this
      pits[i]._tempPos += 50;
      if (pits[i]._tempPos > 1000) {
        for (int j = 0; j < pits[i]._size; j++) {
          leds[pits[i]._pos + j] = colors[j];
        }
        if (pits[i]._tempPos > 2000) {
          pits[i]._tempPos = 0;
        }
      } else {
        for (int j = 0; j < pits[i]._size; j++) {
          leds[pits[i]._pos + j] = colors[j + 1];
        }
      }
    }
  }
}

void drawExit() {
  leds[NUM_LEDS-1] = CRGB::White;
  leds[NUM_LEDS-2] = CRGB::White;
  leds[NUM_LEDS-2].fadeLightBy(150);
  leds[NUM_LEDS-3] = CRGB::White;
  leds[NUM_LEDS-3].fadeLightBy(220);
}

