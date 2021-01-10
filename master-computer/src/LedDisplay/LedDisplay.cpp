#include "LedDisplay.h"

// class Screen
// {
// public:
//   int8_t digits[3][4];
//   uint8_t prio_counter[3];
//   uint8_t prio_counter_enable[3];
//   uint8_t it;
//   TM1637 tm1637;

//   bool master_on;

//   Screen()
//     :tm1637(CLK_SCREEN, DATA_SCREEN)
//   {
    
//     tm1637.init();
//     tm1637.set(BRIGHT_TYPICAL); //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
//     master_on = false;

//     this->clearAllPrirorities();
//     for (uint8_t i = 0; i < 3; i++)
//     {
//       prio_counter_enable[i] = false;
//     }
//     it = 0;
//   }
//   void display(uint8_t digits[], uint8_t priority = 0)
//   {
//     if (priority >= 3)
//       return; //ERROR
//     for (uint8_t i = 0; i < 4; i++)
//     {
//       this->digits[priority][i] = digits[i];
//     }
//   }
//   void displayAndUpdate(uint8_t digits[], uint8_t priority = 0)
//   {
//     display(digits, priority);
//     updateScreen();
//   }
//   void clearAllPrirorities()
//   {
//     for (uint8_t j = 0; j < 3; j++)
//     {
//       for (uint8_t i = 0; i < 4; i++)
//       {
//         digits[j][i] = 0x7f;
//       }
//     }
//   }
//   void updateScreen()
//   {
//     if (master_on)
//     {
//       uint8_t chosen_priority = 0;
//       for (int8_t j = 2; j >= 0; j--) // krecimy sie w nieskonczonosc
//       {
//         for (uint8_t i = 0; i < 4; i++)
//         {
//           if (digits[j][i] != 0x7f)
//           {
//             chosen_priority = j;
//             break;
//           }
//         }
//         if(chosen_priority > 0) break;
//       }
//       tm1637.display(digits[chosen_priority]);


//       for (uint8_t i = 0; i < 3; i++)
//       {
//         if (prio_counter_enable[i])
//         {
//           if (prio_counter[i] == it)
//           {
//             clearOnePriority(i);
//             prio_counter_enable[i] = false;
//           }
//         }
//       }
//     }else{
//       int8_t clear_tab[] = {0x7f, 0x7f, 0x7f, 0x7f};
//       tm1637.display(clear_tab);
//     }
//     it++;
//   }
//   void clearOnePriority(uint8_t prio)
//   {
//     if (prio >= 3)
//       return;
//     for (uint8_t i = 0; i < 4; i++)
//     {
//       digits[prio][i] = 0x7f;
//     }
//   }
//   void off(void)
//   {
//     master_on = false;
//     // this->updateScreen();
//   }
//   void on(void)
//   {
//     master_on = true;
//     // this->updateScreen();
//   }
//   void toggle(void)
//   {
//     master_on = master_on ? false : true;
//   }
//   void displayForTime(uint8_t digits[], uint8_t time, uint8_t priority = 0)
//   {
//     for (uint8_t i = 0; i < 4; i++)
//     {
//       this->digits[priority][i] = digits[i];
//     }
//     prio_counter[priority] = (time + it) % 256;
//     prio_counter_enable[priority] = true;
//     // update_screen();
//   }
//   void displayForTime(uint16_t num, uint8_t time, uint8_t priority = 0)
//   {
//     uint8_t digits1[4];
//     digits1[0] = (num / 1000) % 10;
//     digits1[1] = num / 100;
//     digits1[2] = (num / 10) % 10;
//     digits1[3] = (num % 10);
//     displayForTime(digits1, time, priority);
//   }
//   void display(uint16_t num, uint8_t priority)
//   {
//     digits[priority][0] = (num / 1000) % 10;
//     digits[priority][1] = num / 100;
//     digits[priority][2] = (num / 10) % 10;
//     digits[priority][3] = (num % 10);
//   }

//   void displayTemp(float num)
//   {
//     num *= 10;
//     digits[0][0] = (int)num / 100;
//     digits[0][1] = ((int)num / 10) % 10;
//     digits[0][2] = ((int)num % 10);
//     digits[0][3] = 'C';
//     // point(true);
//   }
//     void displayHum(float num)
//   {
//     num *= 10;
//     digits[0][0] = (int)num / 100;
//     digits[0][1] = ((int)num / 10) % 10;
//     digits[0][2] = ((int)num % 10);
//     digits[0][3] = 'H';
//     // point(true);
//   }
// };
Screen::Screen() 
    :tm1637(CLK_SCREEN, DATA_SCREEN)
  {
    
    tm1637.init();
    tm1637.set(BRIGHT_TYPICAL); //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
    master_on = false;

    this->clearAllPrirorities();
    for (uint8_t i = 0; i < 3; i++)
    {
      prio_counter_enable[i] = false;
    }
    it = 0;
  }

void Screen::display(uint8_t digits[], uint8_t priority) 
  {
    if (priority >= 3)
      return; //ERROR
    for (uint8_t i = 0; i < 4; i++)
    {
      this->digits[priority][i] = digits[i];
    }
  }

void Screen::displayAndUpdate(uint8_t digits[], uint8_t priority) 
  {
    display(digits, priority);
    updateScreen();
  }

void Screen::clearAllPrirorities() 
  {
    for (uint8_t j = 0; j < 3; j++)
    {
      for (uint8_t i = 0; i < 4; i++)
      {
        digits[j][i] = 0x7f;
      }
    }
  }

void Screen::updateScreen() 
  {
    if (master_on)
    {
      uint8_t chosen_priority = 0;
      for (int8_t j = 2; j >= 0; j--) // krecimy sie w nieskonczonosc
      {
        for (uint8_t i = 0; i < 4; i++)
        {
          if (digits[j][i] != 0x7f)
          {
            chosen_priority = j;
            break;
          }
        }
        if(chosen_priority > 0) break;
      }
      tm1637.display(digits[chosen_priority]);


      for (uint8_t i = 0; i < 3; i++)
      {
        if (prio_counter_enable[i])
        {
          if (prio_counter[i] == it)
          {
            clearOnePriority(i);
            prio_counter_enable[i] = false;
          }
        }
      }
    }else{
      int8_t clear_tab[] = {0x7f, 0x7f, 0x7f, 0x7f};
      tm1637.display(clear_tab);
    }
    it++;
  }

void Screen::clearOnePriority(uint8_t prio) 
  {
    if (prio >= 3)
      return;
    for (uint8_t i = 0; i < 4; i++)
    {
      digits[prio][i] = 0x7f;
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

void Screen::displayForTime(uint8_t digits[], uint8_t time, uint8_t priority) 
  {
    for (uint8_t i = 0; i < 4; i++)
    {
      this->digits[priority][i] = digits[i];
    }
    prio_counter[priority] = (time + it) % 256;
    prio_counter_enable[priority] = true;
    // update_screen();
  }

void Screen::displayForTime(uint16_t num, uint8_t time, uint8_t priority) 
  {
    uint8_t digits1[4];
    digits1[0] = (num / 1000) % 10;
    digits1[1] = num / 100;
    digits1[2] = (num / 10) % 10;
    digits1[3] = (num % 10);
    displayForTime(digits1, time, priority);
  }

void Screen::display(uint16_t num, uint8_t priority) 
  {
    digits[priority][0] = (num / 1000) % 10;
    digits[priority][1] = num / 100;
    digits[priority][2] = (num / 10) % 10;
    digits[priority][3] = (num % 10);
  }

void Screen::displayTemp(float num) 
  {
    num *= 10;
    digits[0][0] = (int)num / 100;
    digits[0][1] = ((int)num / 10) % 10;
    digits[0][2] = ((int)num % 10);
    digits[0][3] = 'C';
    // point(true);
  }

void Screen::displayHum(float num) 
  {
    num *= 10;
    digits[0][0] = (int)num / 100;
    digits[0][1] = ((int)num / 10) % 10;
    digits[0][2] = ((int)num % 10);
    digits[0][3] = 'H';
    // point(true);
  }