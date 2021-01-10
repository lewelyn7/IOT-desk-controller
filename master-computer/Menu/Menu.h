#ifndef __MENU_H__
#define __MENU_H__


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
  Menu();
  void setState(MenuStates mstate);
  void setSerial(SerialStates serial);
};
#endif // __MENU_H__