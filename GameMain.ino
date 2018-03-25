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
    void Win() {
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[NUM_LEDS-1-i] = CRGB::Green;
        FastLED.show();
        delay(5);
      }
      _alive = false;
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
    Pit(void) {
      _alive = false;
    }
    void spawnPit(int pos) {
      _alive = true;
      _pos = pos;
    }
    int getPos() {
      return _pos;
    }
    int _pos;
};

Character hero;

Enemy enemy;
Enemy enemy2;
Enemy enemy3;
Pit pit;

void setup() {
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setDither(1);

  Serial.begin(9600);
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(joyButtonPin, INPUT_PULLUP); 

  

  enemy2.Spawn(140,-1,60);
  enemy.Spawn(0,1,90);
  //pit.spawnPit(10);

}

void loop () {
  while (hero._alive) {
    long mm = millis();
    long lastRefreshTime = 0;
    if (mm - lastRefreshTime >= REFRESH_INTERVAL) {
      lastRefreshTime += REFRESH_INTERVAL;
      //drawEnemy(enemy2);
      //drawEnemy(enemy);
      drawExit();
      //drawPit(pit);
      //drawEnemy(enemy3);
      drawCharacter(hero);
    }
    FastLED.show(); 
  }  
}

void getInput(Character &character) {
  //fine tune these numbers
  yPosition = map(analogRead(yPin), 0, 1023, -80, 80);
  joyButtonState = digitalRead(joyButtonPin);
  xPosition = map(analogRead(xPin), 0, 1023, -80, 80);
  Serial.print(yPosition);
  Serial.print("\n");
  Serial.print(xPosition);
  if (xPosition < -4 && (yPosition > -5 && yPosition < 4)) {
    character._color = CRGB::Yellow;
  } else if (xPosition > 0 && (yPosition > -5 && yPosition < 4)) {
    character._color = CRGB::Blue;
  } else {
    character._color = CRGB::Red;
  }
  leds[character._pos] = character._color;
}


void drawEnemy(Enemy &enemy) {
  if (enemy._alive) {
    enemy._tempPos += enemy._speed;
  
    if (enemy._tempPos > 1000) {
      
      enemy._prevPos = enemy._pos;
      enemy._pos += (1 * enemy._dir);
  
      leds[enemy._prevPos] = CRGB::Black;
      leds[enemy._pos] = CRGB::White;
      
      if (enemy._pos == 0) {
        enemy._pos = NUM_LEDS;
        leds[0] = CRGB::Black;
      } else if (enemy._pos == NUM_LEDS) {
        enemy._pos = 0;
      }
      enemy._tempPos = 0; 
    }
  }
  if (inPit(enemy._pos)) {
    enemy.Kill(false);
  }
}

void drawCharacter (Character &character) {
  if (character._alive) {
    getInput(character);
    character._tempPos += abs(yPosition);
    if (character._tempPos > 1000) {
      
      character._prevPos = character._pos;
      character._pos += (1 * (yPosition < 0 ? 1 : -1));
  
      leds[character._prevPos] = CRGB::Black;
      leds[character._pos] = character._color;
      
      if (character._pos < 0) {
        character._pos = 0;
      } else if (character._pos == NUM_LEDS-1) {
        //character._pos = 0;
        //win condition -- code
        character.Win();
      }
      character._tempPos = 0; 
    } 
  }
  if (inPit(character._pos)) {
    character.Die();
  }
}

boolean inPit(int pos) {
  if (pos == pit._pos) {
    return true;
  } else {
    return false;
  }
}

void drawPit(Pit pit) {
  if (pit._alive) {
    //randomly sized pit maybe
    leds[pit._pos] = CRGB::Purple;
    leds[pit._pos+1] = CRGB::Purple;
    leds[pit._pos-1] = CRGB::Purple;
  }
}

void drawExit() {
  leds[NUM_LEDS-1] = CRGB::White;
  leds[NUM_LEDS-2] = CRGB::White;
  leds[NUM_LEDS-2].fadeLightBy(150);
  leds[NUM_LEDS-3] = CRGB::White;
  leds[NUM_LEDS-3].fadeLightBy(220);
}



