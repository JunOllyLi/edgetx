#include "opentx.h"
#include "Arduino.h"

const int LEDPin = BACKLITE_PIN;

int dutyCycle;
/* Setting PWM Properties */
const int PWMFreq = 50000; /* 50 KHz to avoid high pitch noise */
const int PWMChannel = 1;
const int PWMResolution = 10;
const int MAX_DUTY_CYCLE = (int)(pow(2, PWMResolution) - 1);
const int MIN_DUTY_CYCLE = (int)(MAX_DUTY_CYCLE/2);

static bool bkl_enabled = false;
void backlightInit() {
  pinMode(LEDPin, OUTPUT);
  ledcSetup(PWMChannel, PWMFreq, PWMResolution);
  /* Attach the LED PWM Channel to the GPIO Pin */
  ledcAttachPin(LEDPin, PWMChannel);
  backlightEnable(BACKLIGHT_LEVEL_MAX/2);
}

void backlightEnable(uint8_t level) {
  static uint16_t prev_level = 0;
  const uint16_t offset = 200;
  uint16_t l = (((uint16_t)level) * (MAX_DUTY_CYCLE - MIN_DUTY_CYCLE) / BACKLIGHT_LEVEL_MAX) + MIN_DUTY_CYCLE;
  if (l != prev_level) {
    prev_level = l;
    ledcWrite(PWMChannel, l);
  }
  bkl_enabled = true;
}

void backlightDisable() {
  ledcWrite(PWMChannel, MIN_DUTY_CYCLE);
  bkl_enabled = false;
}
uint8_t isBacklightEnabled() {return bkl_enabled;}
