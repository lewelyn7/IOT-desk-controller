
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
  void display(uint8_t digits[], uint8_t timer);
  void display(uint8_t digits[]);
  void display(int number);
  void display(int number, uint8_t time);

  void updateScreen();
  void off(void);
  void on(void);
  void toggle(void);
  void displayTemp(float num);
  void displayHum(float num);
};




#endif // __LEDDISPLAY_H__