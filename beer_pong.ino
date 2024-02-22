#include "Cup.h"
#include "FastLED.h"

#define NUM_LEDS 1
#define DATA_PIN_LEDS 12

CRGB leds[NUM_LEDS];

Cup game[1] = {Cup(A3)};//, Cup(A1, 1, DATA_PIN_LEDS), Cup(A2, 2, DATA_PIN_LEDS), Cup(A3, 3, DATA_PIN_LEDS), Cup(A4, 4, DATA_PIN_LEDS), Cup(A5, 5, DATA_PIN_LEDS), Cup(A6, 6, DATA_PIN_LEDS), Cup(A7, 7, DATA_PIN_LEDS), Cup(A8, 8, DATA_PIN_LEDS), Cup(A9, 9, DATA_PIN_LEDS)};

bool teamLeft = true;
int hitsThisRound = 0;
int firstHitLED = -1;

void setup() {
  Serial.begin(9600);

  FastLED.addLeds<WS2812, DATA_PIN_LEDS, GRB>(leds, NUM_LEDS);
}

void teamSwitch() {
  teamLeft = !teamLeft;
  for (int i=0; i<sizeof game/sizeof game[0]; i++) { game[i].toggleGame(); } // Call toggleGame() for every Cup-object
}

void loop() {
  bool hit = false; // Detects whether any cup was hit this iteration

  // This code is executed for every Cup-object
  for (int i=0; i<sizeof game/sizeof game[0]; i++) {
    // Register a hit through the return value of the loop()-method, but only if the respectie Cup isn't already fully hit (red LED)
    hit = game[i].loop() && ((teamLeft && !game[i].getHitLeft()) || (!teamLeft && !game[i].getHitRight()));

    // If an LED is supposed to glow, it glows red, except it is the LED of the cup that was first hit this round
    if (((teamLeft && game[i].getLedLeft()) || (!teamLeft && game[i].getLedRight())) && firstHitLED != i) {
      leds[i] = CRGB::Red;
    }

  if (hit && hitsThisRound == 0) {  // First hit this round
    firstHitLED = i;                // Register this LED as the LED of the first cup hit this round
    leds[i] = CRGB(255, 150, 0);    // Set the color of the first hit cup's LED to yellow
    hitsThisRound = 1;              // Set the hits this round to 1
  } else if (hit && hitsThisRound == 1) { // Second hit this round
    if (firstHitLED == i) {
      // Code for bomb (bomb animation)
    }
    leds[firstHitLED] = CRGB::Red;                                                            // Set the yellow LED red
    if (teamLeft) { game[firstHitLED].setHitLeft(true); game[i].setHitLeft(true); }           // Register a full hit both team left's hit cups (or only the first if bomb) if it's their turn
    else if (!teamLeft) { game[firstHitLED].setHitRight(true); game[i].setHitRight(true); }   // Register a full hit both team right's hit cups (or only the first if bomb) if it's their turn
    hitsThisRound = 0;                                                                        // Reset the hits this round
    firstHitLED = -1;                                                                         // Set the first hit LED to -1, so no LED is meant, since their indexes is positive
    // Two hit animation
  }
  }
  /*if (digitalRead(TEAM SWITCH BUTTON) == 1) {
    teamSwitch();
  }*/
  FastLED.show();
  if (hit) { delay(500); } // Delay to make sure no Cup registers a hit twice
}
