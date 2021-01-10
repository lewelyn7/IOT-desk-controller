#ifndef __LEDWRAPPER_H__
#define __LEDWRAPPER_H__

#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS 90

void global_set_hsv(CRGB *leds, uint8_t i, uint8_t h, uint8_t s, uint8_t v);
void global_set_rgb(CRGB *leds, uint8_t i, CRGB color);

#endif // __LEDWRAPPER_H__