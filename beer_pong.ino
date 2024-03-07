#include "Cup.h"
#include "FastLED.h"

#define TEAM_SWITCH_BUTTON 30
#define ACTION_BUTTON 36
#define SELECT_BUTTON 33

#define NUM_LEDS 3
#define DATA_PIN_LEDS 12
#define LEDS_PER_CUP 3

CRGB leds[NUM_LEDS];
CRGB real_leds[NUM_LEDS*LEDS_PER_CUP];

Cup game[3] = {Cup(A3), Cup(A4), Cup(A5)};//, Cup(A1, 1, DATA_PIN_LEDS), Cup(A2, 2, DATA_PIN_LEDS), Cup(A3, 3, DATA_PIN_LEDS), Cup(A4, 4, DATA_PIN_LEDS), Cup(A5, 5, DATA_PIN_LEDS), Cup(A6, 6, DATA_PIN_LEDS), Cup(A7, 7, DATA_PIN_LEDS), Cup(A8, 8, DATA_PIN_LEDS), Cup(A9, 9, DATA_PIN_LEDS)};



bool teamLeft = true;
int hitsThisRound = 0;
int firstHitLED = -1;

bool teamSwitchPressed = false;
bool teamSwitchReleased = true;

bool actionPressed = false;
bool actionLongPressed = false;
bool actionLongReleased = true;
const float actionThreshold = 120;
float actionThresholdCounter = 0;

bool selectButtonPressed = false;
bool selectButtonReleased = true;
bool firstRun;
CRGB pastLED;

bool selectMode = false;
int selectIndex = 0;

bool run = true;

void setup() {
  Serial.begin(9600);

  pinMode(TEAM_SWITCH_BUTTON, INPUT_PULLUP);
  pinMode(ACTION_BUTTON, INPUT_PULLUP);
  pinMode(SELECT_BUTTON, INPUT_PULLUP);

  FastLED.addLeds<WS2812, DATA_PIN_LEDS, GRB>(real_leds, NUM_LEDS*LEDS_PER_CUP);

  real_leds[0] = CRGB::Blue;
  FastLED.show();
}

void updateLEDs() {
  int led_index = 0;
  for (int i=0; i<sizeof game/sizeof game[0]; i++) {
    for (int j=0; j<LEDS_PER_CUP; j++) {
      real_leds[led_index] = leds[i];
      led_index++;
    }
  }
  FastLED.show();
}

void teamSwitch() {
  teamLeft = !teamLeft;
  for (int i=0; i<sizeof game/sizeof game[0]; i++) { game[i].toggleGame(); } // Call toggleGame() for every Cup-object
}

void runSelectMode() {
  if (firstRun) {
    pastLED = leds[selectIndex];
    leds[selectIndex] = CRGB::DarkGreen;
    firstRun = false;
  }
  selectButtonPressed = digitalRead(SELECT_BUTTON) == LOW;
  if (selectButtonPressed && selectButtonReleased) {
    leds[selectIndex] = pastLED;
    selectButtonReleased = false;
    selectIndex++;
    if (selectIndex >= sizeof game/sizeof game[0]) {
      selectIndex = 0;
    }
    pastLED = leds[selectIndex];
    leds[selectIndex] = CRGB::DarkGreen;
  } else if (!selectButtonPressed) {
    selectButtonReleased = true;
  }

  if (actionPressed) {
    if (selectIndex == firstHitLED) { firstHitLED = -1; hitsThisRound = 0; }
    if (teamLeft) {
      bool newState = !game[selectIndex].getLedLeft();
      game[selectIndex].setLedLeft(newState);
      game[selectIndex].setHitLeft(newState);
      if (game[selectIndex].getLedLeft()) { leds[selectIndex] = CRGB::Red; updateLEDs(); delay(500); pastLED = CRGB::Red; leds[selectIndex] = CRGB::DarkGreen; updateLEDs(); }
      else                                { leds[selectIndex] = CRGB::Black; updateLEDs(); delay(500); pastLED = CRGB::Black; leds[selectIndex] = CRGB::DarkGreen; updateLEDs(); }
    } else {
      bool newState = !game[selectIndex].getLedRight();
      game[selectIndex].setLedRight(newState);
      game[selectIndex].setHitRight(newState);
      if (game[selectIndex].getLedRight()) { leds[selectIndex] = CRGB::Red; updateLEDs(); delay(500); pastLED = CRGB::Red; leds[selectIndex] = CRGB::DarkGreen; updateLEDs(); }
      else                                 { leds[selectIndex] = CRGB::Black; updateLEDs(); delay(500); pastLED = CRGB::Black; leds[selectIndex] = CRGB::DarkGreen; updateLEDs(); }
    }
  }
  
  updateLEDs();
}

void loop() {
  bool hit = false; // Detects whether any cup was hit this iteration

  actionPressed = false;
  if (digitalRead(ACTION_BUTTON) == LOW) {
    actionThresholdCounter++;
  } else {
    if (actionThresholdCounter != 0 && actionThresholdCounter < actionThreshold) {
      actionPressed = true;
      if(!selectMode) { run = !run; }
    }
    actionThresholdCounter = 0;
  }

  if (actionThresholdCounter >= actionThreshold && actionLongReleased) {
    actionLongPressed = true;
    actionLongReleased = false;
    if (run || selectMode ) { selectMode = !selectMode; }
    if (selectMode) { firstRun = true; run = false; }
    else            { run = true; }
  } else if (actionThresholdCounter == 0) {
    actionLongPressed = false;
    actionLongReleased = true;
  }

  if (run) {
    // This code is executed for every Cup-object
    for (int i=0; i<sizeof game/sizeof game[0]; i++) {
      // Register a hit through the return value of the loop()-method, but only if the respectie Cup isn't already fully hit (red LED)
      hit = game[i].loop() && ((teamLeft && !game[i].getHitLeft()) || (!teamLeft && !game[i].getHitRight()));

      // If an LED is supposed to glow, it glows red, except it is the LED of the cup that was first hit this round
      if (((teamLeft && game[i].getLedLeft()) || (!teamLeft && game[i].getLedRight())) && firstHitLED != i) {
        leds[i] = CRGB::Red;
      } else if (firstHitLED != i) {
        leds[i] = CRGB::Black;
      } else {
        leds[i] = CRGB(255, 150, 0);    // Set the color of the first hit cup's LED to yellow
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
      delay(300);
      run = false;
    }
    
    updateLEDs();
    if (hit) { delay(500); } // Delay to make sure no Cup registers a hit twice
    }
  } else if (!selectMode) {
    for (int i=0; i<sizeof game/sizeof game[0]; i++) { leds[i] = CRGB(100, 100, 100); } // Get every cup-LED to glow white
    updateLEDs();
  }

  teamSwitchPressed = digitalRead(TEAM_SWITCH_BUTTON) == LOW;
  if (teamSwitchPressed && teamSwitchReleased && !selectMode) {
    if (hitsThisRound == 1) {
      // Fundamentally same reset code as above
      leds[firstHitLED] = CRGB::Red;
      updateLEDs();
      delay(200);
      leds[firstHitLED] = CRGB::White;
      updateLEDs();
      delay(200);
      leds[firstHitLED] = CRGB::Red;
      updateLEDs();
      if (teamLeft) { game[firstHitLED].setHitLeft(true); }
      else if (!teamLeft) { game[firstHitLED].setHitRight(true); }
      hitsThisRound = 0;
      firstHitLED = -1;
      updateLEDs();
      delay(300);
    }
    run = true;
    teamSwitch();
    teamSwitchReleased = false;
  } else if (!teamSwitchPressed) {
    teamSwitchReleased = true;
  }

  if (selectMode) {
    runSelectMode();
  }
}
