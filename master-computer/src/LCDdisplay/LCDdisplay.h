#ifndef __LCDDISPLAY_H__
#define __LCDDISPLAY_H__
#include <LiquidCrystal_I2C.h>

class LCDdisplay{
    private:
        bool backlight = false;
        bool master_on = false;
    public:
        LiquidCrystal_I2C lcd;
        LCDdisplay();
        void on();
        void off();
        void backlight_off();
        void backlight_on();
        void autoscroll();
        void noAutoscroll(); 
        void setCursor(uint8_t, uint8_t); 
        void cursor_on();
        void cursor_off();
        void printstr(const char[]);
};
#endif // __LCDDISPLAY_H__