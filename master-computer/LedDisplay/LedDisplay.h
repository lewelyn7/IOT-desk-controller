#ifndef __LEDDISPLAY_H__
#define __LEDDISPLAY_H__

#include <Arduino.h>
#include <TM1637.h>
#define CLK_SCREEN 0
#define DATA_SCREEN 2

class Screen
{
public:
  int8_t digits[3][4];
  uint8_t prio_counter[3];
  uint8_t prio_counter_enable[3];
  uint8_t it;
  bool master_on;

  Screen();
  void display(uint8_t digits[], uint8_t priority = 0);
  void displayAndUpdate(uint8_t digits[], uint8_t priority = 0);
  void clearAllPrirorities();
  void updateScreen();
  void clearOnePriority(uint8_t prio);
  void off(void);
  void on(void);
  void toggle(void);
  void displayForTime(uint8_t digits[], uint8_t time, uint8_t priority = 0);
  void displayForTime(uint16_t num, uint8_t time, uint8_t priority = 0);
  void display(uint16_t num, uint8_t priority);
  void displayTemp(float num);
  void displayHum(float num);
};
#endif // __LEDDISPLAY_H__