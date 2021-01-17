#ifndef __LEDWRAPPER_H__
#define __LEDWRAPPER_H__

#include "LedWrapper.h"
CRGB converter = CRGB();
void global_set_hsv(CRGB *leds, uint8_t i, uint8_t h, uint8_t s, uint8_t v)
{
  if (i >= NUM_LEDS)
  {
    return;
  }
  if (i < 59)
  {
    converter.setHSV(h % 256, s % 256, v % 256);
    leds[i].setRGB((uint8_t)((double)converter.r*0.7), converter.g, (uint8_t)((double)converter.b*0.7));
  }
  else
  {

    converter.setHSV(h % 256, s % 256, v % 256);
    leds[i].setRGB((uint8_t)((double)converter.r), converter.g*0.9, (uint8_t)((double)converter.b*0.55));

  }
}
void global_set_rgb(CRGB *leds, uint8_t i, CRGB color)
{
  if (i >= NUM_LEDS)
  {
    return;
  }
  if (i < 59)
  {
    leds[i].setRGB(color.r, color.g, color.b);
  }
  else
  {
    leds[i].setRGB(color.r, color.g, color.b);
  }
}
#endif // __LEDWRAPPER_H__