#include "Menu.h"
Menu::Menu() 
{
state = MenuStates::Starting;
    serial_state = SerialStates::Ready;
    led_screen_mode = General;
    _update_scr_mode();
}

void Menu::setState(MenuStates mstate) 
{
      state = mstate;

}

void Menu::setSerial(SerialStates serial) 
{
      serial_state = serial;

}

void Menu::selectLedScrMode(LedScreenModes led_screen_mode) 
{
      
}

void Menu::tickModeSelectorRight() 
{
      mode_enc_tick++;
      if(mode_enc_tick == 250){
            mode_enc_tick = 0;
      }
      _calc_mode_from_tick(false);
}

void Menu::tickModeSelectorLeft() 
{
      mode_enc_tick--;
      if(mode_enc_tick == 0){
            mode_enc_tick = 250;
      }
      _calc_mode_from_tick(true);
      

}
void Menu::_calc_mode_from_tick(bool left_tick){
      if(this->mode_enc_tick%5 == 0){
            if( left_tick == true){
                  this->led_screen_mode = (LedScreenModes)(this->led_screen_mode-1);
                  if(this->led_screen_mode == FIRST){
                        this->led_screen_mode = LAST;
                        this->led_screen_mode = (LedScreenModes)(this->led_screen_mode-1);
                  }
            }else{
                  this->led_screen_mode = (LedScreenModes)(this->led_screen_mode+1);
                  if(this->led_screen_mode == LAST){
                        this->led_screen_mode = FIRST;
                        this->led_screen_mode = (LedScreenModes)(this->led_screen_mode+1);
                  }                 
            }
            _update_scr_mode();

      }
}

void Menu::_update_scr_mode() 
{
int8_t mode_name[4] = {0x7f, 0x7f, 0x7f, 0x7f};
    switch (this->led_screen_mode)
    {
    case Time:
          mode_name[3] = 0;
          break;
    case Timer:
          mode_name[3] = 1;
          break;
    case Temp:
          mode_name[3] = 3;
          break;
    default: //General
          mode_name[3] = 2;
          break;
    }
      ScreenLayer* lay = &(*screen).notifications;
      screen->display((int)this->led_screen_mode,20, lay);
      screen->setMode(this->led_screen_mode);
      mqtt->update_led_scr_mode();
}