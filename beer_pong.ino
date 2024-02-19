#include "Cup.h"
#include "FastLED.h"

#define NUM_LEDS 1
#define DATA_PIN_LEDS 12

CRGB leds[NUM_LEDS];

Cup game[1] = {Cup(A3)};//, Cup(A1, 1, DATA_PIN_LEDS), Cup(A2, 2, DATA_PIN_LEDS), Cup(A3, 3, DATA_PIN_LEDS), Cup(A4, 4, DATA_PIN_LEDS), Cup(A5, 5, DATA_PIN_LEDS), Cup(A6, 6, DATA_PIN_LEDS), Cup(A7, 7, DATA_PIN_LEDS), Cup(A8, 8, DATA_PIN_LEDS), Cup(A9, 9, DATA_PIN_LEDS)};

bool teamLeft = false;
int hitThisRound = 0;
int firstHitLED = -1;

void setup() {
  Serial.begin(9600);

  FastLED.addLeds<WS2812, DATA_PIN_LEDS, GRB>(leds, NUM_LEDS);
}

void teamSwitch() {
  teamLeft = !teamLeft;
  for (int i=0; i<sizeof game/sizeof game[0]; i++) { game[i].toggleGame(); }
}

void loop() {
  for (int i=0; i<sizeof game/sizeof game[0]; i++) {
    bool hit = game[i].loop();

    // Code
    if ((teamLeft && game[i].ledLeft()) || (!teamLeft && game[i].ledRight()) && firstHitLED != i) {
      leds[i] = CRGB::Red;
    } else if (firstHitLED != i) {
      leds[i] = CRGB::Black;
    }

  if (hit && hitThisRound == 0) {
    firstHit = i;
    leds[i] = CRGB::Yellow;
    hitThisRound = 1;
  } else if (hit && hitThisRound == 1) {
    if (firstHit == i) {
      // Code for bomb (bomb animation)
    }
    //leds[firstHit] = CRGB::Red;
    hitThisRound = 0;
    firstHitLED = -1;
    // Two hit animation
  }
  }
  /*if (digitalRead(TEAM SWITCH BUTTON) == 1) {
    teamSwitch();
  }*/
  FastLED.show();
}