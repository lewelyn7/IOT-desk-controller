#ifndef __MENU_H__
#define __MENU_H__
#include <Arduino.h>
#include "../globals.h"
#include "../LedDisplay/LedDisplay.h"
enum class MenuStates
{
  Starting,
  Started,
};
enum class SerialStates
{
  Ready,
  ColorReadingHSV
};



class Menu
{
public:
  MenuStates state;
  SerialStates serial_state;
  LedScreenModes led_screen_mode;
  uint8_t mode_enc_tick = 0;
  Menu();
  void setState(MenuStates mstate);
  void setSerial(SerialStates serial);
  void selectLedScrMode(LedScreenModes led_screen_mode);
  void tickModeSelectorRight();
  void tickModeSelectorLeft();
  private:
  void _calc_mode_from_tick(bool left_tick);
  void _update_scr_mode();
};


#endif // __MENU_H__