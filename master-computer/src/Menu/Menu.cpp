#include "Menu.h"
Menu::Menu() 
{
      state = MenuStates::Starting;
    serial_state = SerialStates::Ready;
}

void Menu::setState(MenuStates mstate) 
{
      state = mstate;

}

void Menu::setSerial(SerialStates serial) 
{
      serial_state = serial;

}