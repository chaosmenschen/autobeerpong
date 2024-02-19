#ifndef Cup_h
#define Cup_h
#include "Arduino.h"
#include "FastLED.h"
class Cup {
public:
	Cup(int sensorPin);
  bool getHitLeft();
	void setHitLeft(bool isHit);
  bool getHitRight();
  void setHitRight(bool isHit);
  bool getLedLeft();
  void setLedLeft(bool isOn);
  bool getLedRight();
  void setLedRight(bool isOn);
  void toggleGame();
  int loop();
private:
	int _sensorPin;
  int _isHitLeft;
  int _isHitRight;
  bool _firstGame;
  bool _ledLeft;
  bool _ledRight;
};
#endif