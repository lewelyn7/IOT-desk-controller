#include "Panel.h"

Panel::Panel(PanelHandler *handler) 
{
        this->handler = handler;
    for (int i = 0; i < PANEL_LEDS_NUM; i++)
      this->leds[i] = false;
    for (int i = 0; i < PANEL_SWITCHES_NUM; i++)
      this->switches[i] = false;
}

void Panel::led_on(uint8_t num) 
  {
    if(synchronization_mode) return;
    if (num >= PANEL_LEDS_NUM)
      return; //ERROR
    leds[num] = true;
    switch (num)
    {
    case 0:
      pcSerial1.write(D1N);
      break;
    case 1:
      pcSerial1.write(D2N);
      break;
    case 2:
      pcSerial1.write(D3N);
      break;
    case 3:
      pcSerial1.write(D4N);
      break;
    case 4:
      pcSerial1.write(D5N);
      break;
    case 5:
      pcSerial1.write(D6N);
      break;
    }
  }

void Panel::led_off(uint8_t num) 
  {
    if(synchronization_mode) return;
    if (num >= PANEL_LEDS_NUM)
      return; //ERROR
    leds[num] = false;
    switch (num)
    {
    case 0:
      pcSerial1.write(D1F);
      break;
    case 1:
      pcSerial1.write(D2F);
      break;
    case 2:
      pcSerial1.write(D3F);
      break;
    case 3:
      pcSerial1.write(D4F);
      break;
    case 4:
      pcSerial1.write(D5F);
      break;
    case 5:
      pcSerial1.write(D6F);
      break;
    }
  }

void Panel::led_toggle(uint8_t num) 
 {
    if (num >= PANEL_LEDS_NUM)
      return; //ERROR
    if (leds[num])
    {
      led_off(num);
    }
    else
    {
      led_on(num);
    }
  }
void Panel::switch_on(uint8_t num) 
  {
    if (num >= PANEL_SWITCHES_NUM)
      return; //ERROR
    switches[num] = true;
    switch (num)
    {
    case 0:
      handler->s1_on();
      break;
    case 1:
      handler->s2_on();
      break;
    case 2:
      handler->s3_on();
      break;
    case 3:
      handler->s4_on();
      break;
    case 4:
      handler->s5_on();
      break;
    case 5:
      handler->s6_on();
      break;
    case 6:
      handler->s7_on();
      break;
    case 7:
      handler->s8_on();
      break;
    }
  }
void Panel::switch_off(uint8_t num) 
  {
    if (num >= PANEL_SWITCHES_NUM)
      return; //ERROR
    switches[num] = false;
    switch (num)
    {
    case 0:
      handler->s1_off();
      break;
    case 1:
      handler->s2_off();
      break;
    case 2:
      handler->s3_off();
      break;
    case 3:
      handler->s4_off();
      break;
    case 4:
      handler->s5_off();
      break;
    case 5:
      handler->s6_off();
      break;
    case 6:
      handler->s7_off();
      break;
    case 7:
      handler->s8_off();
      break;
    }
  }

void Panel::switch_toggle(uint8_t num) 
  {
    if (num >= PANEL_SWITCHES_NUM)
      return; //ERROR
    if (switches[num])
    {
      switch_off(num);
    }
    else
    {
      switch_on(num);
    }
  }

void Panel::enc_left(uint8_t num) 
  {
    switch (num)
    {
    case 0:
      handler->e1_left();
      break;
    case 1:
      handler->e2_left();
      break;
    case 2:
      handler->e3_left();
      break;
    case 3:
      handler->e4_left();
      break;
    default:
      break; //ERROR
    }
  }

void Panel::enc_right(uint8_t num) 
  {
    switch (num)
    {
    case 0:
      handler->e1_right();
      break;
    case 1:
      handler->e2_right();
      break;
    case 2:
      handler->e3_right();
      break;
    case 3:
      handler->e4_right();
      break;
    default:
      break; //ERROR
    }
  }

void Panel::enc_btn(uint8_t num) 
  {
    switch (num)
    {
    case 0:
      handler->e1_btn();
      break;
    case 1:
      handler->e2_btn();
      break;
    case 2:
      handler->e3_btn();
      break;
    case 3:
      handler->e4_btn();
      break;
    default:
      break; //ERROR
    }
  }

void Panel::btn(uint8_t num) 
  {
    switch (num)
    {
    case 0:
      handler->btn1();
      break;
    case 1:
      handler->btn2();
      break;
    case 2:
      handler->btn3();
      break;
    case 3:
      handler->btn4();
      break;
    default:
      break; //ERROR
    }
  }

void Panel::send_led_state() 
  {
    for (uint8_t i = 0; i < PANEL_LEDS_NUM; i++)
    {
      if (leds[i])
      {
        pcSerial1.write(D1N + (i * 2));
      }
      else
      {
        pcSerial1.write(D1F + (i * 2));
      }
    }
  }

void Panel::get_state() 
{
    pcSerial1.write(GET_SWITCHES_STATE);
}
void Panel::switches_reset() 
  {
    synchronization_mode = true;
    for (int i = 0; i < PANEL_SWITCHES_NUM; i++)
    {
      switch_off(i);
    }
    synchronization_mode = false;
  }



  void PanelHandler::s1_on()
{
  animationManager->master_layer->On();
  panel->led_on(0);
}
void PanelHandler::s1_off()
{
  animationManager->master_layer->Off();
  panel->led_off(0);
}
void PanelHandler::s1_toggle()
{
}
void PanelHandler::s2_on()
{
  
  screen->on();
  panel->led_on(1);
}
void PanelHandler::s2_off()
{
  screen->off();
  panel->led_off(1);

}
void PanelHandler::s2_toggle()
{
}
void PanelHandler::s3_on()
{
  digitalWrite(LAPTOP_FAN, HIGH);
  panel->led_on(2);
}
void PanelHandler::s3_off()
{
  digitalWrite(LAPTOP_FAN, LOW);
  panel->led_off(2);

}
void PanelHandler::s3_toggle()
{
}
void PanelHandler::s4_on()
{
}
void PanelHandler::s4_off()
{
}
void PanelHandler::s4_toggle()
{
}
void PanelHandler::s5_on()
{
}
void PanelHandler::s5_off()
{
}
void PanelHandler::s5_toggle()
{
}
void PanelHandler::s6_on()
{
}
void PanelHandler::s6_off()
{
}
void PanelHandler::s6_toggle()
{
}
void PanelHandler::s7_on()
{
  animationManager->notify_layer->on();
  panel->led_on(5);
}
void PanelHandler::s7_off()
{
  animationManager->notify_layer->off();
  panel->led_off(5);
}
void PanelHandler::s7_toggle()
{
}
void PanelHandler::s8_on()
{
}
void PanelHandler::s8_off()
{
}
void PanelHandler::s8_toggle()
{
}
void PanelHandler::e1_left()
{
  animationManager->get_current()->setH(animationManager->get_current()->h - 2);
  uint8_t val = animationManager->get_current()->h;
  screen->displayForTime(val, 40, 1);
}
void PanelHandler::e1_right()
{
  animationManager->get_current()->setH(animationManager->get_current()->h + 2);
  uint8_t val = animationManager->get_current()->h;
  screen->displayForTime(val, 40, 1);
}
void PanelHandler::e1_btn()
{
}
void PanelHandler::e2_left()
{
}
void PanelHandler::e2_right()
{
}
void PanelHandler::e2_btn()
{
}
void PanelHandler::e3_left()
{
  animationManager->get_current()->setS(animationManager->get_current()->s - 5);
  uint8_t val = animationManager->get_current()->s;
  screen->displayForTime(val, 40, 1);
}
void PanelHandler::e3_right()
{
  animationManager->get_current()->setS(animationManager->get_current()->s + 5);
  uint8_t val = animationManager->get_current()->s;
  screen->displayForTime(val, 40, 1);
}
void PanelHandler::e3_btn()
{
}
void PanelHandler::e4_left()
{
  animationManager->get_current()->setV(animationManager->get_current()->v - 5);
  uint8_t val = animationManager->get_current()->v;
  screen->displayForTime(val, 40, 1);
}
void PanelHandler::e4_right()
{
  animationManager->get_current()->setV(animationManager->get_current()->v + 5);
  uint8_t val = animationManager->get_current()->v;
  screen->displayForTime(val, 40, 1);
}
void PanelHandler::e4_btn()
{
}
void PanelHandler::btn1()
{
  animationManager->notify_layer->mute_toggle();
}
void PanelHandler::btn2()
{
  animationManager->notify_layer->blink_toggle();
}
void PanelHandler::btn3()
{
  animationManager->previous();
}
void PanelHandler::btn4()
{
  animationManager->next();
}