#include "Arduino.h"
#include "Cup.h"
#include "FastLED.h"
Cup::Cup(int sensorPin) {
	_sensorPin = sensorPin;
  _isHitLeft = false;
  _isHitRight = false;
  _leftGame = true;
  _ledLeft = false;
  _ledRight = false;
}
bool Cup::getHitLeft(){
  return _isHitLeft;
}
void Cup::setHitLeft(bool isHit){
  _isHitLeft = isHit;
}
bool Cup::getHitRight(){
  return _isHitRight;
}
void Cup::setHitRight(bool isHit){
  _isHitRight = isHit;
}
void Cup::toggleGame() {
  _leftGame = !_leftGame;
}
bool Cup::getLedLeft(){
  return _ledLeft;
}
void Cup::setLedLeft(bool isOn){
  _ledLeft = isOn;
}
bool Cup::getLedRight(){
  return _ledRight;
}
void Cup::setLedRight(bool isOn){
  _ledRight = isOn;
}
bool Cup::loop() {
  if (analogRead(_sensorPin) < 400 && _leftGame) {
    _ledLeft = true;
    return true;
  } else if (analogRead(_sensorPin) < 400 && _leftGame == false){
    _ledRight = true;
    return true;
  } else {
    //_isHitLeft = false;
    //_isHitRight = false;
    return false;
  }  
}
