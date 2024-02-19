#include "Cup.h"
#include "FastLED.h"

#define NUM_LEDS 1
#define DATA_PIN_LEDS 12

CRGB leds[NUM_LEDS];

Cup game[1] = {Cup(A3)};//, Cup(A1, 1, DATA_PIN_LEDS), Cup(A2, 2, DATA_PIN_LEDS), Cup(A3, 3, DATA_PIN_LEDS), Cup(A4, 4, DATA_PIN_LEDS), Cup(A5, 5, DATA_PIN_LEDS), Cup(A6, 6, DATA_PIN_LEDS), Cup(A7, 7, DATA_PIN_LEDS), Cup(A8, 8, DATA_PIN_LEDS), Cup(A9, 9, DATA_PIN_LEDS)};

bool teamLeft = true;
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
  bool hit = false;
  for (int i=0; i<sizeof game/sizeof game[0]; i++) {
    hit = game[i].loop() && ((teamLeft && !game[i].getHitLeft()) || (!teamLeft && !game[i].getHitRight()));

    // Code
    if (((teamLeft && game[i].getLedLeft()) || (!teamLeft && game[i].getLedRight())) && firstHitLED != i) {
      leds[i] = CRGB::Red;
    }

  if (hit && hitThisRound == 0) {
    firstHitLED = i;
    leds[i] = CRGB(255, 150, 0);
    hitThisRound = 1;
  } else if (hit && hitThisRound == 1) {
    if (firstHitLED == i) {
      // Code for bomb (bomb animation)
    }
    leds[firstHitLED] = CRGB::Red;
    if (teamLeft) { game[firstHitLED].setHitLeft(true); game[i].setHitLeft(true); }
    else if (!teamLeft) { game[firstHitLED].setHitRight(true); game[i].setHitRight(true); }
    hitThisRound = 0;
    firstHitLED = -1;
    // Two hit animation
  }
  }
  /*if (digitalRead(TEAM SWITCH BUTTON) == 1) {
    teamSwitch();
  }*/
  FastLED.show();
  if (hit) { delay(500); }
}
