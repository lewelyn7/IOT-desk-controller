
#include <Arduino.h>
#include <TM1637.h>
#define CLK_SCREEN 0
#define DATA_SCREEN 2
#ifndef __LEDDISPLAY_H__
#define __LEDDISPLAY_H__

class ScreenLayer{
  public:
    int8_t digits[4];
    uint8_t timer;
    bool timer_enabled;
    bool visibility;
};
enum LedScreenModes{
  FIRST = 0,
  Time, Timer, General,
   Temp, LAST
};
#define LAYERS_NUMBER 4
#define LAYERS_NUMBER_ALL 6
class Screen
{
public:
  ScreenLayer *main;
  ScreenLayer notifications;
  ScreenLayer errors;
  ScreenLayer temp;
  ScreenLayer time;
  ScreenLayer timer;
  ScreenLayer general;
  ScreenLayer *all_main[LAYERS_NUMBER_ALL];
  TM1637 tm1637;
  int8_t *tmp_digits;
  int8_t clear_digits[4] = {0x7f, 0x7f, 0x7f, 0x7f};
  bool master_on;

  Screen();
  void clearAlldigits(ScreenLayer * layer);
  void display(int8_t digits[], uint8_t timer, ScreenLayer*layer = (ScreenLayer*)NULL);
  void display(int8_t digits[], ScreenLayer * layer = (ScreenLayer*)NULL);
  void display(int number, ScreenLayer * layer = (ScreenLayer*)NULL);
  void display(int number, uint8_t time, ScreenLayer * layer = (ScreenLayer*)NULL);

  void updateScreen();
  void off(void);
  void on(void);
  void toggle(void);
  void displayTemp(float num);
  void displayHum(float num);
  void displayTime(uint8_t first, uint8_t second);
  void setMode(LedScreenModes mode);
};




#endif // __LEDDISPLAY_H__