#include "Arduino.h"
#include "LED.h"
#include "Button.h"

// Pin Definitions
#define LEDB_PIN_VIN  2
#define LEDG_PIN_VIN  3
#define LEDR_PIN_VIN  4
#define LEDY_PIN_VIN  5
#define BTN1_PIN      6
#define BTN2_PIN      7
#define BTN3_PIN      8
#define BTN4_PIN      9

// LED and Button Objects
LED leds[4] = {
  LED(LEDB_PIN_VIN), // Blue
  LED(LEDG_PIN_VIN), // Green
  LED(LEDR_PIN_VIN), // Red
  LED(LEDY_PIN_VIN)  // Yellow
};

Button buttons[4] = {
  Button(BTN1_PIN),
  Button(BTN2_PIN),
  Button(BTN3_PIN),
  Button(BTN4_PIN)
};

// Game Settings
const int MAX_SEQUENCE_LENGTH = 100;
int sequence[MAX_SEQUENCE_LENGTH];
int currentLength = 1;
int userIndex = 0;
bool waitingForInput = false;
unsigned long lastActionTime = 0;
const int delayBetweenSteps = 500;

// Game State
void playSequence();
bool readUserInput();
void gameOver();

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0)); // seed randomness

  for (int i = 0; i < 4; i++) {
    buttons[i].init();
    leds[i].off();
  }

  // Start the first sequence
  sequence[0] = random(0, 4);
  playSequence();
}

void loop() {
  if (waitingForInput) {
    if (readUserInput()) {
      userIndex++;
      if (userIndex >= currentLength) {
        delay(500);
        currentLength++;
        if (currentLength >= MAX_SEQUENCE_LENGTH) {
          Serial.println("You won!");
          while (true); // Freeze
        }
        sequence[currentLength - 1] = random(0, 4);
        userIndex = 0;
        playSequence();
      }
    }
  }
}

void playSequence() {
  Serial.print("Playing sequence of length ");
  Serial.println(currentLength);
  for (int i = 0; i < currentLength; i++) {
    int idx = sequence[i];
    leds[idx].on();
    delay(300);
    leds[idx].off();
    delay(200);
  }
  waitingForInput = true;
  userIndex = 0;
}

bool readUserInput() {
  for (int i = 0; i < 4; i++) {
    if (buttons[i].onPress()) {
      leds[i].on();
      delay(150);
      leds[i].off();

      if (i != sequence[userIndex]) {
        gameOver();
        return false;
      }
      return true;
    }
  }
  return false;
}

void gameOver() {
  Serial.println("Wrong input! Game Over.");
  for (int j = 0; j < 3; j++) {
    for (int i = 0; i < 4; i++) leds[i].on();
    delay(200);
    for (int i = 0; i < 4; i++) leds[i].off();
    delay(200);
  }
  currentLength = 1;
  userIndex = 0;
  sequence[0] = random(0, 4);
  playSequence();
}
