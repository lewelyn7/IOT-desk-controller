#include "LCDdisplay.h"
LCDdisplay::LCDdisplay():lcd(0x27,16,2)
{
    lcd.init(); 
    backlight_off();
    //off();
}

void LCDdisplay::on() 
{
    master_on = true;
    lcd.display();
    if(backlight) lcd.backlight();
}

void LCDdisplay::off() 
{
    master_on = false;
   lcd.noBacklight();
   lcd.noDisplay();
}

void LCDdisplay::backlight_off() 
{
    lcd.noBacklight();
    backlight = false;
}

void LCDdisplay::backlight_on() 
{
    if(master_on) lcd.backlight();
    backlight = true;
}

void LCDdisplay::autoscroll() 
{
    
}

void LCDdisplay::noAutoscroll() 
{
    
}

void LCDdisplay::setCursor(uint8_t x, uint8_t y) 
{
    lcd.setCursor(x, y);
}

void LCDdisplay::cursor_on() 
{
    lcd.cursor_on();
}

void LCDdisplay::cursor_off() 
{
    lcd.cursor_off();
}

void LCDdisplay::printstr(const char str[]) 
{
    lcd.printstr(str);
}
