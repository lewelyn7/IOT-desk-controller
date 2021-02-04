#ifndef __PANEL_H__
#define __PANEL_H__

#include "../globals.h"
#include "../utils/utils.h"
#include "../animation/animation.h"
#include "../LedDisplay/LedDisplay.h"
#include "../MQTTCommunicator/MQTTCommunicator.h"
#include "../Menu/Menu.h"
#include "../LCDdisplay/LCDdisplay.h"
#include <Arduino.h>
#define PANEL_LEDS_NUM 6
#define PANEL_SWITCHES_NUM 8

class PanelHandler
{
public:
  virtual void s1_on();
  virtual void s1_off();
  virtual void s1_toggle();
  virtual void s2_on();
  virtual void s2_off();
  virtual void s2_toggle();
  virtual void s3_on();
  virtual void s3_off();
  virtual void s3_toggle();
  virtual void s4_on();
  virtual void s4_off();
  virtual void s4_toggle();
  virtual void s5_on();
  virtual void s5_off();
  virtual void s5_toggle();
  virtual void s6_on();
  virtual void s6_off();
  virtual void s6_toggle();
  virtual void s7_on();
  virtual void s7_off();
  virtual void s7_toggle();
  virtual void s8_on();
  virtual void s8_off();
  virtual void s8_toggle();
  virtual void e1_left();
  virtual void e1_right();
  virtual void e1_btn();
  virtual void e2_left();
  virtual void e2_right();
  virtual void e2_btn();
  virtual void e3_left();
  virtual void e3_right();
  virtual void e3_btn();
  virtual void e4_left();
  virtual void e4_right();
  virtual void e4_btn();
  virtual void btn1();
  virtual void btn2();
  virtual void btn3();
  virtual void btn4();
};
class Panel
{
public:
  PanelHandler *handler;
  bool leds[PANEL_LEDS_NUM];
  bool switches[PANEL_SWITCHES_NUM];
  bool synchronized = false;
  bool synchronization_mode = false;
  Panel(PanelHandler *handler);

  void led_on(uint8_t num);

  void led_off(uint8_t num);

  void led_toggle(uint8_t num);

  void switch_on(uint8_t num);

  void switch_off(uint8_t num);

  void switch_toggle(uint8_t num);

  void enc_left(uint8_t num);

  void enc_right(uint8_t num);

  void enc_btn(uint8_t num);

  void btn(uint8_t num);

  void send_led_state();

  void get_state();

  void switches_reset();

};




#endif // __PANEL_H__