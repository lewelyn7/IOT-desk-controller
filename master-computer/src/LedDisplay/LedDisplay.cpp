#include "LedDisplay.h"

Screen::Screen() 
    :tm1637(CLK_SCREEN, DATA_SCREEN)
  {
    
    tm1637.init();
    tm1637.set(BRIGHT_TYPICAL); //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
    master_on = false;
    main = &general;
    all_main[0] = &general;
    all_main[1] = &time;
    all_main[2] = &temp;
    all_main[3] = &timer;
    all_main[4] = &notifications;
    all_main[5] = &errors;

    for(uint8_t i = 0; i < LAYERS_NUMBER; i++){
      clearAlldigits(all_main[i]);
      all_main[i]->visibility = true;
      all_main[i]->timer_enabled = false;

    }
    clearAlldigits(&notifications);
    notifications.visibility = false;
    notifications.timer_enabled = false;
    clearAlldigits(&errors);
    errors.visibility = false;
    errors.timer_enabled = false;

    this->time.visibility = true;
  }
void Screen::clearAlldigits(ScreenLayer * layer){
  for(uint8_t i = 0; i < 4; i++){
    layer->digits[i] = 0x7f;
  }
}


void Screen::updateScreen() 
  {
    tmp_digits = clear_digits;
    if (master_on)
    {

      if(main->visibility){
        tmp_digits = main->digits;        
      }else{
        tmp_digits = clear_digits;
      }
      if(notifications.visibility){
        tmp_digits = notifications.digits;
      }
      if(errors.visibility){
        tmp_digits = errors.digits;
      }

    }else{
      tmp_digits[0] = 0x7f;
      tmp_digits[1] = 0x7f;
      tmp_digits[2] = 0x7f;
      tmp_digits[3] = 0x7f;
     
    }
    tm1637.display(tmp_digits);
    for(uint8_t i = 0; i < LAYERS_NUMBER_ALL; i++){
      if(all_main[i]->timer_enabled &&  all_main[i]->timer > 0){
        all_main[i]->timer--;
      }
      if(all_main[i]->timer_enabled &&  all_main[i]->timer == 0){
        all_main[i]->visibility = false;
        all_main[i]->timer_enabled = false;
      }      
    }

  }


void Screen::off(void) 
  {
    master_on = false;
    // this->updateScreen();
  }

void Screen::on(void) 
  {
    master_on = true;
    // this->updateScreen();
  }

void Screen::toggle(void) 
  {
    master_on = master_on ? false : true;
  }
  
void Screen::display(uint8_t digits[]) 
{
  general.digits[0] = digits[0];
  general.digits[1] = digits[1];
  general.digits[2] = digits[2];
  general.digits[3] = digits[3];
  general.visibility = true;
}
void Screen::display(uint8_t digits[], uint8_t time) 
{
  this->display(digits);
  general.timer_enabled = true;
  general.timer = time;
}

void Screen::display(int number) 
{
    general.digits[0] = number/ 1000;
    general.digits[1] = (number/ 100) %10;
    general.digits[2] = (number/ 10) %10;
    general.digits[3] =  number%10;
    general.visibility = true;

}

void Screen::display(int number, uint8_t time) 
{
  this->display(number);
  general.timer_enabled = true;
  general.timer = time;

}

void Screen::displayTemp(float num) 
  {
    num *= 10;
    temp.digits[0] = (int)num / 100;
    temp.digits[1] = ((int)num / 10) % 10;
    temp.digits[2] = ((int)num % 10);
    temp.digits[3] = 'C';
    // point(true);
  }

void Screen::displayHum(float num) 
{

}

void Screen::displayTime(uint8_t first, uint8_t second) 
{
  time.digits[0] = first / 10;
  time.digits[1] = first % 10;
  time.digits[2] = second / 10;
  time.digits[3] = second % 10;  
}

void Screen::setTimeMode(void) 
{
  main = &time;
}