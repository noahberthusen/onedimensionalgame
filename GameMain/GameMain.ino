#include <FastLED.h>
#include <Character.h>
#include <Enemy.h>
#include <Pit.h>
#include <SinEnemy.h>
#include <Boss.h>

// LED setup
#define NUM_LEDS 149
#define DATA_PIN 5
#define LED_TYPE WS2812B
#define BRIGHTNESS 60
#define COLOR_ORDER GRB

// Game
#define REFRESH_INTERVAL 1000
CRGB leds[NUM_LEDS];
long ms = 0;

int xPin = A1;
int yPin = A0;
int joyButtonPin = 2;

int xPosition = 0;
int yPosition = 0;
int joyButtonState = 0;

Character character;
Boss boss;
// game objects
const int numEnemies = 3;
Enemy Enemies[numEnemies] =
{
  Enemy(), Enemy(), Enemy()
};
const int numSin = 3;
SinEnemy SinEnemies[numSin] =
{
  SinEnemy(), SinEnemy(), SinEnemy()
};
const int numPits = 3;
Pit pits[numPits] =
{
  Pit(), Pit(), Pit()
};

int level = 7;


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
  ms = millis();
  drawCharacter(character);
    switch (level) {
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

void setupLevel(int level) {
  switch (level) {
    case 1:
      break;
    case 2:
      Enemies[0].Spawn(130, -1, 50);
      break;
    case 3:
      pits[0].spawnPit(50, 3);
      pits[1].spawnPit(100, 5);
      break;
    case 4:
      Enemies[0].Spawn(130, -1, 50);
      pits[1].spawnPit(110, 3);
      SinEnemies[0].Spawn(80, 100, 50, 150);
      break;
    case 5:
      Enemies[0].Spawn(40, 1, 100);
      SinEnemies[0].Spawn(70, 120, 60, 100);
      SinEnemies[1].Spawn(120, 130, 80, 80);
      pits[0].spawnPit(70, 4);
      break;
    case 6:
      Enemies[0].Spawn(40, 1, 150);
      Enemies[1].Spawn(100, 1, 150);
      Enemies[2].Spawn(140, 1, 150);
      SinEnemies[0].Spawn(20, 130, 20, 100);
      break;
    case 7:
      boss.Spawn(130, -1, 20);
      break;
    default:
      break;
  }
}

void getInput(Character &character) {
  yPosition = -map(analogRead(yPin), 0, 1023, -80, 80);
  yPosition = (yPosition <= 3 && yPosition >= -3) ? 0 : yPosition;
  joyButtonState = digitalRead(joyButtonPin);
  xPosition = map(analogRead(xPin), 0, 1023, -80, 80);
  character._dir = yPosition < 0 ? -1 : 1;

  if (xPosition < -4) {
    character._color = CRGB::Yellow;
    if (!character._jump && character._pos < 138) {
      character._jumpTime = millis();
      character._jump = true;
      leds[2] = CRGB::Black;
      leds[character._pos] = CRGB::Black;
      if (yPosition > 20) {
        character._pos += 7;
      } else if (yPosition < -20) {
        character._pos -= 7;
      }
      enemyInteraction();
    }
  } else if (xPosition > 0 && (yPosition > -10 && yPosition < 10)) {
    if (!character._shield) {
      character._shield = true;
      character._shieldTime = millis();
      character._color = CRGB(102, 51, 153);
      leds[0] = CRGB::Black;
    } else {
      if (ms - character._shieldTime > 2000) {
        character._color = CRGB::White;
      }
    }
  } else {
    character._color = CRGB::White;
    if (ms - character._shieldTime > 9000 || character._shieldTime == 0) {
      character._shield = false;
      leds[0] = CRGB(102, 51, 153);
    }
    if (ms - character._jumpTime > 1000 || character._jumpTime == 0) {
      character._jump = false;
      leds[2] = CRGB::Yellow;
    }
    if (ms - character._attackTime > 9000 || character._attackTime == 0) {
      leds[1] = CRGB::Red;
    }
    if (leds[1] == CRGB(0, 0, 0)) {
      character._attack = false;
      for (int i = 1; i < 4; i++) {
        leds[character._pos + i] = CRGB::Black;
        leds[character._pos - i] = CRGB::Black;
      }
    }
    if (joyButtonState == 0 && yPosition == 0) {
      if (leds[1] == CRGB(255, 0, 0)) {
        leds[1] = CRGB::Black;
        character._attack = true;
        character._attackTime = millis();
        for (int i = 1; i < 4; i++) {
          leds[character._pos + i] = CRGB::Red;
          leds[character._pos - i] = CRGB::Red;
        }
      }
    }
  }
  leds[character._pos] = character._color;
}

void enemyInteraction() {
  for (int i = 0; i < numEnemies; i++) {
    if (Enemies[i]._alive) {
      if (character._pos == Enemies[i]._pos) {
        character.Die(leds);
      }
      if (abs(character._pos - Enemies[i]._pos) <= 3 && character._attack) {
        Enemies[i].Kill(true, leds);
      }
    }
  }
  for (int i = 0; i < numSin; i++) {
    if (SinEnemies[i]._alive) {
      if (character._pos == SinEnemies[i]._pos) {
        character.Die(leds);
      }
    }
    if (abs(character._pos - SinEnemies[i]._pos) <= 3 && character._attack) {
      SinEnemies[i].Kill(true, leds);
    }
  }
  for (int i = 0; i < numPits; i++) {
    if (pits[i]._alive) {
      for (int j = 0; j < pits[i]._size; j++) {
        if (character._pos == pits[i]._pos + j) {
          character.Die(leds);
        }
      }
    }
  }
  if (boss._alive) {
    if (character._pos == boss._pos) {
        character.Die(leds);
    }
    for (int i = 0; i < boss._lives; i++) {
      if (character._pos == boss._pos+i) {
        character.Die(leds);
      }
    }
    if ((abs(boss._pos - character._pos) <= 3 || abs(boss._pos + boss._lives - character._pos <= 3)) && character._attack) {
      boss._lives -= 1;
      boss._speed += 30;
      boss._pos = random(10,130);
      if (boss._lives == 0) {
        boss.Kill(true, leds);
        drawExit();
      }
    }
  }
}

void drawEnemy() {
  for (int i = 0; i < numEnemies; i++) {
    if (Enemies[i]._alive) {
      Enemies[i]._tempPos += Enemies[i]._speed;

      if (Enemies[i]._tempPos > 1000) {

        Enemies[i]._prevPos = Enemies[i]._pos;
        Enemies[i]._pos += (1 * Enemies[i]._dir);

        if (Enemies[i]._pos < 3) {
          Enemies[i]._prevPos = 3;
          Enemies[i]._pos = NUM_LEDS - 1;
        } else if (Enemies[i]._pos > NUM_LEDS - 1) {
          Enemies[i]._prevPos = NUM_LEDS - 1;
          Enemies[i]._pos = 3;
        }

        leds[Enemies[i]._prevPos] = CRGB::Black;
        leds[Enemies[i]._pos] = CRGB::Red;

        Enemies[i]._tempPos = 0;
      }
    }
  }
}

void drawSinEnemy() {
  for (int i = 0; i < numSin; i++) {
    if (SinEnemies[i]._alive) {
      SinEnemies[i]._tempPos += SinEnemies[i]._speed;

      if (SinEnemies[i]._tempPos > 1000) {

        SinEnemies[i]._prevPos = SinEnemies[i]._pos;
        SinEnemies[i]._pos += (1 * SinEnemies[i]._dir);

        if (SinEnemies[i]._pos == SinEnemies[i]._upper || SinEnemies[i]._pos == SinEnemies[i]._lower) {
          SinEnemies[i]._dir *= -1;
        }

        leds[SinEnemies[i]._prevPos] = CRGB::Black;
        leds[SinEnemies[i]._pos] = CRGB::Red;
        SinEnemies[i]._tempPos = 0;
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
    if (boss._teleport > 3000) {
      boss._pos = random(10, 130);
      boss._dir *= -1;
      boss._teleport = 0;
      FastLED.clear();
    }

    //drawing boss
    for (int i = 0; i < boss._lives; i++) {
      leds[boss._prevPos + i] = CRGB::Black;
    }
    for (int i = 0; i < boss._lives; i++) {
      leds[boss._pos + i] = CRGB::Green;
    }
    
    if (boss._pos <= 10) {
      boss._dir = 1;
    } else if (boss._pos > NUM_LEDS - 10) {
      boss._dir = -1;
    }
  }
}

void drawCharacter (Character &character) {
  if (character._alive) {
    getInput(character);
    enemyInteraction();

    if (character._pos >= NUM_LEDS - 4) {
      Win();
    } else if (character._pos <= 3) {
      character._pos = 3;
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

void Win() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[NUM_LEDS - 1 - i] = CRGB::Green;
    FastLED.show();
    delay(5);
  }
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[NUM_LEDS - 1 - i] = CRGB::Black;
    FastLED.show();
    delay(5);
  }
  for (int i = 0; i < numEnemies; i++) {
    Enemies[i].Kill(false, leds);
  }
  for (int i = 0; i < numPits; i++) {
    pits[i].Kill();
  }
  for (int i = 0; i < numSin; i++) {
    SinEnemies[i].Kill(false, leds);
  }
  level += 1;
  character._pos = 3;
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
  leds[NUM_LEDS - 1] = CRGB::White;
  leds[NUM_LEDS - 2] = CRGB::White;
  leds[NUM_LEDS - 2].fadeLightBy(150);
  leds[NUM_LEDS - 3] = CRGB::White;
  leds[NUM_LEDS - 3].fadeLightBy(220);
}
