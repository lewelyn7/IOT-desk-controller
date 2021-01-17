// #pragma once

#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include <LinkedList.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include "../LedWrapper/LedWrapper.h"
#include <Arduino.h>


// class Animation;
// class StaticAnimation;
// class BlinkingAnimation;
// class  TravelingDotAnimation;
// class StartAnimation;
// class NotificationLayer;
// class MasterLayer;
// class AnimationsManager;

class Animation
{
public:
  uint8_t fps;
  uint8_t h;
  uint8_t s;
  uint8_t v;
  CRGB *leds;
  Animation(CRGB *leds, uint8_t frames_per_sec);;
  virtual void tick();
  virtual bool done();
  virtual void setH(uint8_t val);
  virtual void setS(uint8_t val);
  virtual void setV(uint8_t val);
  void setHSV(uint8_t h1, uint8_t s1, uint8_t v1);
  void clear_all();
};
class StaticAnimation : public Animation
{
public:
  uint16_t i;

  StaticAnimation(CRGB *leds, uint8_t frames);;


  void tick();;

  void updateAll(void);;

};
class BlinkingAnimation : public Animation
{
public:
  uint16_t i;
  uint8_t speed;
  enum stages
  {
    INCREASE_K,
    DECREASE_K,
    LOW_K,
    HIGH_K
  };
  stages stage;
  BlinkingAnimation(CRGB *leds, uint8_t frames);;

  void tick();;

};
class TravelingDotAnimation : public Animation
{
public:
  uint16_t i;
  enum stages
  {
    START,
    FORWARD,
    REVERSE
  };
  stages stage;
  TravelingDotAnimation(CRGB *leds, uint8_t frames);

  void tick();
};
class MasterLayer : public Animation
{
public:
  uint16_t i;
  bool master_switch;

  bool muted = true;
  enum states
  {
    OFFing,
    OFF,
    ONing,
    ON
  };
  states state;
  MasterLayer(CRGB *leds, uint8_t frames);
  void tick();
  void Off(void);
  void On(void);
  void Toggle(void);
};
class NotificationLayer : public Animation
{
public:
  uint16_t i;
  bool on_switch;
  bool blinker;

  bool muted = true;
  bool discord_muted = true;
  NotificationLayer(CRGB *leds, uint8_t frames);

  void tick();
  void mute();
  void unmute();
  void discord_mute();
  void mute_toggle();
  void on();
  void off();
  void toggle();
  void blink(uint16_t i, CRGB color, uint8_t speed, uint8_t start, uint8_t stop);
  void blink_on();
  void blink_off();
  void blink_toggle();
};


class StartAnimation : public Animation
{
private:
  bool done_status;

public:
  uint16_t i;
  uint8_t blink_counter;
  enum stages
  {
    LoadingStrip,
    BlinkDown,
    BlinkUp,
    Ready
  };
  stages stage;
  StartAnimation(CRGB *leds, uint8_t frames);
  void tick();
  bool done();
};
class AnimationsManager
{
private:

  int curr_index = 0;

public:

  Animation *curr;
  StaticAnimation *static_anim;
  BlinkingAnimation *blinking_anim;
  TravelingDotAnimation *tavel_dot_anim;
  StartAnimation * start_anim;
  NotificationLayer *notify_layer;
  MasterLayer *master_layer;
  LinkedList<Animation *> *anim_list;
  AnimationsManager(CRGB *leds);

  void add(Animation *anim);

  void tick();

  void next();

  void previous();

  Animation *get_current(void);

};



#endif // __ANIMATION_H__