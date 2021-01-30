
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
#define GENERAL_LAYER_BUF_SIZE 64
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
  LedScreenModes curr_mode;
  const char * temp_str = "temp";
  const char * time_str = "time";
  const char * timer_str = "timer";
  const char * general_str = "general";
  TM1637 tm1637;
  int8_t *tmp_digits;
  int8_t clear_digits[4] = {0x7f, 0x7f, 0x7f, 0x7f};
  bool master_on;
  char general_buffer[GENERAL_LAYER_BUF_SIZE];
  char general_buffer_idx = 0;
  bool general_buffer_replay = false;
  uint8_t time_iter = 0;
  uint8_t time_iter_setting = 40;
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
  const char* get_mode_str_representation();
  void addToGeneralBuff(char c);
  void copyToGeneralBuff(char * x, uint8_t size);
};




#endif // __LEDDISPLAY_H__