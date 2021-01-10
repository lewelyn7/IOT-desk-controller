#include "Menu.h"
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
  Menu()
  {
    state = MenuStates::Starting;
    serial_state = SerialStates::Ready;
  }
  void setState(MenuStates mstate)
  {
    state = mstate;
  }
  void setSerial(SerialStates serial)
  {
    serial_state = serial;
  }
};