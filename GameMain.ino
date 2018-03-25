#include <FastLED.h>
class Character;
class Enemy;

// LED setup
#define NUM_LEDS 149
#define DATA_PIN 5
#define LED_TYPE WS2812B
#define BRIGHTNESS 75
#define COLOR_ORDER GRB

// Game
#define REFRESH_INTERVAL 16

CRGB leds[NUM_LEDS];
int xPin = A1;
int yPin = A0;
int joyButtonPin = 2;
int buttonPin = 3;

int xPosition = 0;
int yPosition = 0;
int joyButtonState = 0;
int buttonState = 0;

class Character
{
  private:
    
  public:
    //variables
    int _pos;
    int _prevPos;
    boolean _alive;
    CRGB _color;
    long _tempPos;

    //constructor
    Character() {
      _color = CRGB::Red;
      _tempPos = 0;
      _pos = 0;
      _alive = true;
      //_ability = CRGB::Red;
    }
    void Die() {
      // animation
      // lives minus one
      _pos = 0;
    }
};

class Enemy
{
  private:
    int _starting;
  public:
    int _dir;
    int _pos;
    int _prevPos;
    boolean _alive;
    int _speed;
    long _tempPos;
    
    Enemy(void) {
      _alive = true;
      _tempPos = 0;
      _pos = 0;
    }
    void Spawn(int pos, int dir, int speed_) {
      _starting = pos;
      _pos = pos;
      _dir = dir;
      _alive = true;
      _speed = speed_;
    }
    void Kill(boolean reason) {
      // reason is killed by character
      if (reason) {
        _alive = false;
      } else {
        Spawn(_starting, _dir, _speed);
      }
    }
};

class Pit
{
  private:
  public:
    boolean _alive;
    int _tempPos;
    Pit(void) {
      _alive = false;
    }
    void spawnPit(int pos) {
      _tempPos = 0;
      _alive = true;
      _pos = pos;
    }
    int getPos() {
      return _pos;
    }
    int _pos;
};

Character hero;

// game objects
const int numEnemies = 2;
Enemy enemies[numEnemies] = 
{
  Enemy(), Enemy()
};
const int numPits = 2;
Pit pits[numPits] = 
{
  Pit(), Pit()
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
  pinMode(buttonPin, INPUT);

  setupLevel(level);
}



void loop () {

  long mm = millis();
  long lastRefreshTime = 0;
    if (mm - lastRefreshTime >= REFRESH_INTERVAL) {
      lastRefreshTime += REFRESH_INTERVAL;
      switch(level) {
        case 1:
          drawCharacter(hero);
          drawPit();
          //drawEnemy();
          drawExit();
          break;
        case 2:
          drawCharacter(hero);
          drawExit();
          break;
      }
      FastLED.show();
    }
    
      
}

void setupLevel(int level) {
  switch(level) {
    case 1:
      pits[0].spawnPit(100);
      //enemies[0].Spawn(130,-1,50);
      return;
    case 2:
      return;
    case 3:
    default:
      return;
  }
}

void getInput(Character &character) {
  //fine tune these numbers
  yPosition = map(analogRead(yPin), 0, 1023, -80, 80);
  joyButtonState = digitalRead(joyButtonPin);
  xPosition = map(analogRead(xPin), 0, 1023, -80, 80);
  buttonState = digitalRead(buttonPin);

  if (xPosition < -4 && (yPosition > -10 && yPosition < 10)) {
    character._color = CRGB::Yellow;
    if (buttonState == 1) {
      
    }
  } else if (xPosition > 0 && (yPosition > -10 && yPosition < 10)) {
    character._color = CRGB::Blue;
    if( buttonState == 1) {
      character._color = CRGB(102, 51, 153);
    }    
  } else {
    character._color = CRGB::Red;
  }
  leds[character._pos] = character._color;
}


void drawEnemy() {
  for (int i = 0; i < numEnemies; i++) {
    if (enemies[i]._alive) {
      enemies[i]._tempPos += enemies[i]._speed;
    
      if (enemies[i]._tempPos > 1000) {
        
        enemies[i]._prevPos = enemies[i]._pos;
        enemies[i]._pos += (1 * enemies[i]._dir);
    
        leds[enemies[i]._prevPos] = CRGB::Black;
        leds[enemies[i]._pos] = CRGB::White;
        
        if (enemies[i]._pos == 0) {
          enemies[i]._pos = NUM_LEDS;
          leds[0] = CRGB::Black;
        } else if (enemies[i]._pos == NUM_LEDS) {
          enemies[i]._pos = 0;
        }
        enemies[i]._tempPos = 0; 
      }
    }
  }
}

void drawCharacter (Character &character) {
  if (character._alive) {
    getInput(character);
    enemyInteraction();
    pitInteraction();
    character._tempPos += abs(yPosition);
    if (character._tempPos > 1000) {
      
      character._prevPos = character._pos;
      character._pos += (1 * (yPosition < 0 ? 1 : -1));
  
      leds[character._prevPos] = CRGB::Black;
      leds[character._pos] = character._color;
      
      if (character._pos < 0) {
        character._pos = 0;
      }
      if (hero._pos == NUM_LEDS-1) {
        hero._pos = 0;
        FastLED.clear();
        Win();
      }
      character._tempPos = 0; 
    } 
  }
  
}

void enemyInteraction() {
  for (int i = 0; i < numEnemies; i++) {
    if (enemies[i]._alive) {
      if (hero._pos == enemies[i]._pos) {
        if (hero._color == CRGB(102, 51, 153)) {
          
        } else if (hero._color == CRGB(255, 0, 0)) {
          hero.Die();
        } else {
          hero.Die();
        }
      }
      //add more to tell about colors
    }
  }
}

void pitInteraction() {
  for (int i = 0; i < numPits; i++) {
    if (pits[i]._alive) {
      if (hero._pos == pits[i]._pos-2 || hero._pos == pits[i]._pos+2) {
        hero.Die();
      }
    }
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
  level += 1;
  setupLevel(level);
  FastLED.clear();
}

void drawPit() {
  for (int i = 0; i < numPits; i++) {
    if (pits[i]._alive) {
      //refine this
      pits[i]._tempPos += 50;
      if (pits[i]._tempPos > 1000) {
        leds[pits[i]._pos-2] = CRGB::Red;
        leds[pits[i]._pos-1] = CRGB::DarkRed;
        leds[pits[i]._pos] = CRGB::Red;
        leds[pits[i]._pos+1] = CRGB::DarkRed;
        leds[pits[i]._pos+2] = CRGB::Red;
  
        if (pits[i]._tempPos > 2000) {
          pits[i]._tempPos = 0;
        }
      } else {
        leds[pits[i]._pos-2] = CRGB::DarkRed;
        leds[pits[i]._pos-1] = CRGB::Red;
        leds[pits[i]._pos] = CRGB::DarkRed;
        leds[pits[i]._pos+1] = CRGB::Red;
        leds[pits[i]._pos+2] = CRGB::DarkRed;
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

