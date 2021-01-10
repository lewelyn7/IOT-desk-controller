#include "LedWrapper.h"
void global_set_hsv(CRGB *leds, uint8_t i, uint8_t h, uint8_t s, uint8_t v)
{
  if (i >= NUM_LEDS)
  {
    return;
  }
  if (i < 59)
  {
    leds[i].setHSV((h + 6) % 256, s % 256, v % 256);
  }
  else
  {
    leds[i].setHSV(h % 256, s % 256, v % 256);
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