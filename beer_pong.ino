#include "Cup.h"
#include "FastLED.h"

// Button pins
#define TEAM_SWITCH_BUTTON 30
#define ACTION_BUTTON 36
#define SELECT_BUTTON 33

// LED settings
#define NUM_LEDS 3 // Should be the same as the length of game[]
#define DATA_PIN_LEDS 12
#define LEDS_PER_CUP 3 // 16 is the correct value

CRGB leds[NUM_LEDS];
CRGB real_leds[NUM_LEDS*LEDS_PER_CUP];
// One cup has multiple LEDs, so one leds[] object describes multiple real_leds[] objects

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

  // Configure buttons
  pinMode(TEAM_SWITCH_BUTTON, INPUT_PULLUP);
  pinMode(ACTION_BUTTON, INPUT_PULLUP);
  pinMode(SELECT_BUTTON, INPUT_PULLUP);

  FastLED.addLeds<WS2812, DATA_PIN_LEDS, GRB>(real_leds, NUM_LEDS*LEDS_PER_CUP);
}

void updateLEDs() {
  int led_index = 0;
  for (int i=0; i<sizeof game/sizeof game[0]; i++) {
    // For every leds[] object (same length as game[])...
    for (int j=0; j<LEDS_PER_CUP; j++) {
      // ... set LEDS_PER_CUP LEDs on the strip to the color of this leds[] object
      real_leds[led_index] = leds[i];
      led_index++; // Go to next leds[] object
    }
  }
  FastLED.show();
}

void teamSwitch() {
  teamLeft = !teamLeft;
  for (int i=0; i<sizeof game/sizeof game[0]; i++) { game[i].toggleGame(); } // Call toggleGame() for every Cup-object
}

void runSelectMode() {
  /*
  Every time the select button is pressed, the pastLED
  is set to the color of the selected LED. Then, the color
  of the selected LED is set to DarkGreen.
  */
  if (firstRun) {
    // This has to be executed the first time
    // the function runs to initiate the first
    // selected LED
    pastLED = leds[selectIndex];
    leds[selectIndex] = CRGB::DarkGreen;
    firstRun = false;
  }

  selectButtonPressed = digitalRead(SELECT_BUTTON) == LOW;
  if (selectButtonPressed && selectButtonReleased) {
    leds[selectIndex] = pastLED; // Reset the dark green LED to its original color
    selectButtonReleased = false;

    selectIndex++; // Move to the next LED
    if (selectIndex >= sizeof game/sizeof game[0]) {
      selectIndex = 0; // If out of bounds of LEDs, start over at index 0
    }

    pastLED = leds[selectIndex];
    leds[selectIndex] = CRGB::DarkGreen;
  } else if (!selectButtonPressed) {
    selectButtonReleased = true;
  }

  if (actionPressed) {
    // Toggle code
    if (selectIndex == firstHitLED) { firstHitLED = -1; hitsThisRound = 0; } // Make sure there aren't any errors in case the selected cup is the first hit cup and the only cup that was hit this round
    if (teamLeft) {
      bool newState = !game[selectIndex].getLedLeft();
      game[selectIndex].setLedLeft(newState);
      game[selectIndex].setHitLeft(newState);
      if (game[selectIndex].getLedLeft()) { leds[selectIndex] = CRGB::Red; updateLEDs(); delay(500); pastLED = CRGB::Red; leds[selectIndex] = CRGB::DarkGreen; updateLEDs(); }      // Toggle animation
      else                                { leds[selectIndex] = CRGB::Black; updateLEDs(); delay(500); pastLED = CRGB::Black; leds[selectIndex] = CRGB::DarkGreen; updateLEDs(); }  // Toggle animation
    } else {
      bool newState = !game[selectIndex].getLedRight();
      game[selectIndex].setLedRight(newState);
      game[selectIndex].setHitRight(newState);
      if (game[selectIndex].getLedRight()) { leds[selectIndex] = CRGB::Red; updateLEDs(); delay(500); pastLED = CRGB::Red; leds[selectIndex] = CRGB::DarkGreen; updateLEDs(); }     // Toggle animation
      else                                 { leds[selectIndex] = CRGB::Black; updateLEDs(); delay(500); pastLED = CRGB::Black; leds[selectIndex] = CRGB::DarkGreen; updateLEDs(); } // Toggle animation
    }
  }
  
  updateLEDs();
}

void bomb(int cupIndex) {
  // Animation
  for (int i=0; i<sizeof game/sizeof game[0]; i++) { leds[i] = CRGB::Black; } // Turn off all LEDs
  leds[0] = CRGB(100, 100, 100);
  leds[6] = CRGB(100, 100, 100);
  leds[9] = CRGB(100, 100, 100);
  updateLEDs();
  delay(1000/12);
  leds[0] = CRGB::Black;
  leds[6] = CRGB::Black;
  leds[9] = CRGB::Black;
  leds[1] = CRGB(100, 100, 100);
  leds[7] = CRGB(100, 100, 100);
  leds[3] = CRGB(100, 100, 100);
  updateLEDs();
  delay(1000/12);
  leds[1] = CRGB::Black;
  leds[7] = CRGB::Black;
  leds[3] = CRGB::Black;
  leds[5] = CRGB(100, 100, 100);
  leds[8] = CRGB(100, 100, 100);
  leds[2] = CRGB(100, 100, 100);
  updateLEDs();
  delay(1000/12);
  leds[5] = CRGB::Black;
  leds[8] = CRGB::Black;
  leds[2] = CRGB::Black;
  leds[0] = CRGB(100, 100, 100);
  leds[6] = CRGB(100, 100, 100);
  leds[9] = CRGB(100, 100, 100);
  updateLEDs();
  delay(1000/12);
  leds[1] = CRGB(100, 100, 100);
  leds[7] = CRGB(100, 100, 100);
  leds[3] = CRGB(100, 100, 100);
  updateLEDs();
  delay(1000/12);
  leds[0] = CRGB::Black;
  leds[6] = CRGB::Black;
  leds[9] = CRGB::Black;
  leds[5] = CRGB(100, 100, 100);
  leds[8] = CRGB(100, 100, 100);
  leds[2] = CRGB(100, 100, 100);
  updateLEDs();
  delay(1000/12);
  leds[1] = CRGB::Black;
  leds[7] = CRGB::Black;
  leds[3] = CRGB::Black;
  leds[4] = CRGB(100, 100, 100);
  updateLEDs();
  delay(1000/12);
  leds[5] = CRGB::Black;
  leds[8] = CRGB::Black;
  leds[2] = CRGB::Black;
  updateLEDs();
  delay(1000/3);
  leds[4] = CRGB::Black;
  updateLEDs();
  delay(1000/24);
  leds[4] = CRGB(100, 100, 100);
  updateLEDs();
  delay(1000/24);
  leds[4] = CRGB::Black;
  updateLEDs();
  delay(1000/24);
  leds[4] = CRGB(100, 100, 100);
  updateLEDs();
  delay(1000/24);
  leds[4] = CRGB::Black;
  updateLEDs();
  delay(1000/24);
  leds[4] = CRGB::Red;
  updateLEDs();
  delay(1000/12);
  leds[1] = CRGB::Red;
  leds[5] = CRGB::Red;
  leds[7] = CRGB::Red;
  leds[8] = CRGB::Red;
  leds[3] = CRGB::Red;
  leds[2] = CRGB::Red;
  updateLEDs();
  delay(1000/12);
  leds[4] = CRGB::Black;
  updateLEDs();
  delay(1000/12);
  leds[0] = CRGB::Red;
  leds[6] = CRGB::Red;
  leds[9] = CRGB::Red;
  updateLEDs();
  delay(1000/12);
  leds[1] = CRGB::Black;
  leds[5] = CRGB::Black;
  leds[7] = CRGB::Black;
  leds[8] = CRGB::Black;
  leds[3] = CRGB::Black;
  leds[2] = CRGB::Black;
  updateLEDs();
  delay(1000/8);
  leds[0] = CRGB::Black;
  leds[6] = CRGB::Black;
  leds[9] = CRGB::Black;
  updateLEDs();
  delay(1000/12);

  // Check which cup has been hit and mark the adjacent cups as hit
  switch (cupIndex) {
    case 0:
      if (teamLeft) {
        game[1].setHitLeft(true);
        game[1].setLedLeft(true);
        game[2].setHitLeft(true);
        game[2].setLedLeft(true);
      } else {
        game[1].setHitRight(true);
        game[1].setLedRight(true);
        game[2].setHitRight(true);
        game[2].setLedRight(true);
      }
      break;
    case 1:
      if (teamLeft) {
        game[0].setHitLeft(true);
        game[0].setLedLeft(true);
        game[2].setHitLeft(true);
        game[2].setLedLeft(true);
        game[4].setHitLeft(true);
        game[4].setLedLeft(true);
        game[5].setHitLeft(true);
        game[5].setLedLeft(true);
      } else {
        game[0].setHitRight(true);
        game[0].setLedRight(true);
        game[2].setHitRight(true);
        game[2].setLedRight(true);
        game[4].setHitRight(true);
        game[4].setLedRight(true);
        game[5].setHitRight(true);
        game[5].setLedRight(true);
      }
      break;
    case 2:
      if (teamLeft) {
        game[0].setHitLeft(true);
        game[0].setLedLeft(true);
        game[1].setHitLeft(true);
        game[1].setLedLeft(true);
        game[4].setHitLeft(true);
        game[4].setLedLeft(true);
        game[3].setHitLeft(true);
        game[3].setLedLeft(true);
      } else {
        game[0].setHitRight(true);
        game[0].setLedRight(true);
        game[1].setHitRight(true);
        game[1].setLedRight(true);
        game[4].setHitRight(true);
        game[4].setLedRight(true);
        game[3].setHitRight(true);
        game[3].setLedRight(true);
      }
      break;
    case 3:
      if (teamLeft) {
        game[8].setHitLeft(true);
        game[8].setLedLeft(true);
        game[2].setHitLeft(true);
        game[2].setLedLeft(true);
        game[4].setHitLeft(true);
        game[4].setLedLeft(true);
        game[9].setHitLeft(true);
        game[9].setLedLeft(true);
      } else {
        game[8].setHitRight(true);
        game[8].setLedRight(true);
        game[2].setHitRight(true);
        game[2].setLedRight(true);
        game[4].setHitRight(true);
        game[4].setLedRight(true);
        game[9].setHitRight(true);
        game[9].setLedRight(true);
      }
      break;
    case 4:
      if (teamLeft) {
        game[1].setHitLeft(true);
        game[1].setLedLeft(true);
        game[2].setHitLeft(true);
        game[2].setLedLeft(true);
        game[7].setHitLeft(true);
        game[7].setLedLeft(true);
        game[5].setHitLeft(true);
        game[5].setLedLeft(true);
        game[8].setHitLeft(true);
        game[8].setLedLeft(true);
        game[3].setHitLeft(true);
        game[3].setLedLeft(true);
      } else {
        game[1].setHitRight(true);
        game[1].setLedRight(true);
        game[2].setHitRight(true);
        game[2].setLedRight(true);
        game[7].setHitRight(true);
        game[7].setLedRight(true);
        game[5].setHitRight(true);
        game[5].setLedRight(true);
        game[8].setHitRight(true);
        game[8].setLedRight(true);
        game[3].setHitRight(true);
        game[3].setLedRight(true);
      }
      break;
    case 5:
      if (teamLeft) {
        game[1].setHitLeft(true);
        game[1].setLedLeft(true);
        game[7].setHitLeft(true);
        game[7].setLedLeft(true);
        game[4].setHitLeft(true);
        game[4].setLedLeft(true);
        game[6].setHitLeft(true);
        game[6].setLedLeft(true);
      } else {
        game[1].setHitRight(true);
        game[1].setLedRight(true);
        game[7].setHitRight(true);
        game[7].setLedRight(true);
        game[4].setHitRight(true);
        game[4].setLedRight(true);
        game[6].setHitRight(true);
        game[6].setLedRight(true);
      }
      break;
    case 6:
      if (teamLeft) {
        game[5].setHitLeft(true);
        game[5].setLedLeft(true);
        game[7].setHitLeft(true);
        game[7].setLedLeft(true);
      } else {
        game[5].setHitRight(true);
        game[5].setLedRight(true);
        game[7].setHitRight(true);
        game[7].setLedRight(true);
      }
      break;
    case 7:
      if (teamLeft) {
        game[6].setHitLeft(true);
        game[6].setLedLeft(true);
        game[5].setHitLeft(true);
        game[5].setLedLeft(true);
        game[4].setHitLeft(true);
        game[4].setLedLeft(true);
        game[8].setHitLeft(true);
        game[8].setLedLeft(true);
      } else {
        game[6].setHitRight(true);
        game[6].setLedRight(true);
        game[5].setHitRight(true);
        game[5].setLedRight(true);
        game[4].setHitRight(true);
        game[4].setLedRight(true);
        game[8].setHitRight(true);
        game[8].setLedRight(true);
      }
      break;
    case 8:
      if (teamLeft) {
        game[7].setHitLeft(true);
        game[7].setLedLeft(true);
        game[3].setHitLeft(true);
        game[3].setLedLeft(true);
        game[4].setHitLeft(true);
        game[4].setLedLeft(true);
        game[9].setHitLeft(true);
        game[9].setLedLeft(true);
      } else {
        game[7].setHitRight(true);
        game[7].setLedRight(true);
        game[3].setHitRight(true);
        game[3].setLedRight(true);
        game[4].setHitRight(true);
        game[4].setLedRight(true);
        game[9].setHitRight(true);
        game[9].setLedRight(true);
      }
      break;
    case 9:
      if (teamLeft) {
        game[8].setHitLeft(true);
        game[8].setLedLeft(true);
        game[3].setHitLeft(true);
        game[3].setLedLeft(true);
      } else {
        game[8].setHitRight(true);
        game[8].setLedRight(true);
        game[3].setHitRight(true);
        game[3].setLedRight(true);
      }
      break;
  }
  updateLEDs();
}

void loop() {
  bool hit = false; // Detects whether any cup was hit this iteration

  // The action button counts up a threshold and when it gets released, it checks
  // whether it is still in that threshold or not, resulting in a simple press or long press
  actionPressed = false;
  if (digitalRead(ACTION_BUTTON) == LOW) {
    actionThresholdCounter++;
  } else {
    if (actionThresholdCounter != 0 && actionThresholdCounter < actionThreshold) {
      actionPressed = true;
      if(!selectMode) { run = !run; } // Toggle the LED lock if select mode is not enabled
    }
    actionThresholdCounter = 0;
  }

  if (actionThresholdCounter >= actionThreshold && actionLongReleased) {
    actionLongPressed = true;
    actionLongReleased = false;
    if (run || selectMode ) { selectMode = !selectMode; } // The select mode can only be activated if the LED lock is not enabled
    if (selectMode) { firstRun = true; run = false; }     // If the select mode gets enabled, set firstRun to true and lock the run code
    else            { run = true; }                       // If the select mode gets disabled, unlock the run code
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
        leds[i] = CRGB::Black;          // Turn the LED off if it's not supposed to glow
      } else {
        leds[i] = CRGB(255, 150, 0);    // Set the color of the first hit cup's LED to yellow
      }
    
    if (hit && hitsThisRound == 0) {  // First hit this round
      firstHitLED = i;                // Register this LED as the LED of the first cup hit this round
      leds[i] = CRGB(255, 150, 0);    // Set the color of the first hit cup's LED to yellow
      hitsThisRound = 1;              // Set the hits this round to 1
    } else if (hit && hitsThisRound == 1) { // Second hit this round
      if (firstHitLED == i) {
        bomb(i); // Code for bomb (bomb animation)
      }
      leds[firstHitLED] = CRGB::Red;                                                            // Set the yellow LED red
      if (teamLeft) { game[firstHitLED].setHitLeft(true); game[i].setHitLeft(true); }           // Register a full hit both team left's hit cups (or only the first if bomb) if it's their turn
      else if (!teamLeft) { game[firstHitLED].setHitRight(true); game[i].setHitRight(true); }   // Register a full hit both team right's hit cups (or only the first if bomb) if it's their turn
      hitsThisRound = 0;                                                                        // Reset the hits this round
      firstHitLED = -1;                                                                         // Set the first hit LED to -1, so no LED is meant, since their indexes is positive
      // Two hit animation
      run = false;
    }
    
    updateLEDs();
    if (hit) { delay(500); } // Delay to make sure no Cup registers a hit twice
    }
  } else if (!selectMode) {  // LED lock
    for (int i=0; i<sizeof game/sizeof game[0]; i++) { leds[i] = CRGB(100, 100, 100); } // Get every cup-LED to glow white
    updateLEDs();
  }

  teamSwitchPressed = digitalRead(TEAM_SWITCH_BUTTON) == LOW;
  if (teamSwitchPressed && teamSwitchReleased && !selectMode) {
    if (hitsThisRound == 1) {
      // Fundamentally same reset code as above with a simple hit animation
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
