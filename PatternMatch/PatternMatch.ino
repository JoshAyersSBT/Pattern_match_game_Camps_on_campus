#include "Arduino.h"
#include "LED.h"
#include "Button.h"

// === Pin Definitions ===
#define LED_BLUE_PIN    2
#define LED_GREEN_PIN   3
#define LED_RED_PIN     4
#define LED_YELLOW_PIN  5

#define BTN1_PIN        6
#define BTN2_PIN        7
#define BTN3_PIN        8
#define BTN4_PIN        9

// === Object Declarations ===
LED leds[] = {
  LED(LED_BLUE_PIN),
  LED(LED_GREEN_PIN),
  LED(LED_RED_PIN),
  LED(LED_YELLOW_PIN)
};

Button buttons[] = {
  Button(BTN1_PIN),
  Button(BTN2_PIN),
  Button(BTN3_PIN),
  Button(BTN4_PIN)
};

// === Game State ===
bool prevButtonStates[4] = {false, false, false, false};

const int MAX_SEQUENCE_LENGTH = 100;
int sequence[MAX_SEQUENCE_LENGTH];
int currentLength = 1;
int userIndex = 0;
bool acceptingInput = false;

// === Setup ===
void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0)); // Ensure random sequence

  // Initialize buttons and LEDs
  for (int i = 0; i < 4; i++) {
    buttons[i].init();
    leds[i].off();
  }

  Serial.println("Memory Game Start!");
  sequence[0] = random(0, 4);
  playSequence();
}

void loop() {
  if (!acceptingInput) return;

  for (int i = 0; i < 4; i++) {
    bool currentState = buttons[i].read(); // true if pressed

    // Detect full press-release cycle
    if (prevButtonStates[i] && !currentState) {
      // Button was just released — count it as a press
      flashLED(i); // Feedback

      if (i == sequence[userIndex]) {
        userIndex++;
        if (userIndex >= currentLength) {
          delay(500);
          nextRound();
        }
      } else {
        gameOver();
      }
    }

    prevButtonStates[i] = currentState;
  }
}


// === Game Functions ===
void flashLED(int index) {
  leds[index].on();
  delay(300);
  leds[index].off();
}

void playSequence() {
  Serial.print("Round ");
  Serial.println(currentLength);
  acceptingInput = false;
  delay(500);
  for (int i = 0; i < currentLength; i++) {
    flashLED(sequence[i]);
    delay(200);
  }
  userIndex = 0;
  acceptingInput = true;
}

void nextRound() {
  int newButton;
  do {
    newButton = random(0, 4);
  } while (newButton == sequence[currentLength - 1]); // avoid repeat
  sequence[currentLength] = newButton;
  currentLength++;

  if (currentLength >= MAX_SEQUENCE_LENGTH) {
    Serial.println("You win!");
    while (true);
  }

  playSequence();
}


void gameOver() {
  Serial.println("Wrong! Game Over.");
  acceptingInput = false;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) leds[j].on();
    delay(300);
    for (int j = 0; j < 4; j++) leds[j].off();
    delay(300);
  }
  currentLength = 1;
  sequence[0] = random(0, 4);
  playSequence();
}
