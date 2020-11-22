
#include "FastLED.h"
#include <LinkedList.h>
#include <SoftwareSerial.h>
#include "utils.h"
#include <Thread.h>
#include <ThreadController.h>
#include "TM1637.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define NUM_LEDS 90
#define DATA_PIN 5
#define CLOCK_PIN 13
// #define DEBUG 1

#define CLK_SCREEN 6
#define DATA_SCREEN 7
#define LAPTOP_FAN 9
#define DHTPIN 8
#define DHTTYPE DHT11

Thread dhtThread = Thread();
Thread animationThread = Thread();
Thread synchronizationThread = Thread();
Thread screenThread = Thread();
//Thread serial_task = Thread();
//Thread panel_serial_task = Thread();
SoftwareSerial pcSerial1(11, 10); // rx, tx
ThreadController controll = ThreadController();
TM1637 tm1637(CLK_SCREEN, DATA_SCREEN);
DHT_Unified dht(DHTPIN, DHTTYPE);

CRGB leds[NUM_LEDS];
void global_set_hsv(uint8_t i, uint8_t h, uint8_t s, uint8_t v)
{
  if (i >= NUM_LEDS)
  {
    return;
  }
  if (i < 59)
  {
    leds[i].setHSV((h + 6) % 256, s % 256, v % 256);
  }
  else
  {
    leds[i].setHSV(h % 256, s % 256, v % 256);
  }
}
void global_set_rgb(uint8_t i, CRGB color)
{
  if (i >= NUM_LEDS)
  {
    return;
  }
  if (i < 59)
  {
    leds[i].setRGB(color.r, color.g, color.b);
  }
  else
  {
    leds[i].setRGB(color.r, color.g, color.b);
  }
}
int my_max(int i, int max_val)
{
  if (i > max_val)
  {
    return max_val;
  }
  else
  {
    return i;
  }
}
int my_min(int i, int min_val)
{
  if (i < min_val)
  {
    return min_val;
  }
  else
  {
    return i;
  }
}
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
Menu MENU = Menu();

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
  Panel(PanelHandler *handler)
  {
    this->handler = handler;
    for (int i = 0; i < PANEL_LEDS_NUM; i++)
      this->leds[i] = false;
    for (int i = 0; i < PANEL_SWITCHES_NUM; i++)
      this->switches[i] = false;
  }
  void led_on(uint8_t num)
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
  void led_off(uint8_t num)
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
  void led_toggle(uint8_t num)
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
  void switch_on(uint8_t num)
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
  void switch_off(uint8_t num)
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
  void switch_toggle(uint8_t num)
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
  void enc_left(uint8_t num)
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
  void enc_right(uint8_t num)
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
  void enc_btn(uint8_t num)
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
  void btn(uint8_t num)
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
  void send_led_state()
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
  void get_state()
  {
    pcSerial1.write(GET_SWITCHES_STATE);
  }
  void switches_reset()
  {
    synchronization_mode = true;
    for (int i = 0; i < PANEL_SWITCHES_NUM; i++)
    {
      switch_off(i);
    }
    synchronization_mode = false;
  }
};
Panel *panel;

class Animation
{
public:
  uint8_t fps;
  uint8_t h;
  uint8_t s;
  uint8_t v;
  Animation(uint8_t frames_per_sec)
  {
    fps = frames_per_sec;
    h = 100;
    s = 100;
    v = 100;
  }
  virtual void tick()
  {
  }
  virtual bool done()
  {
    return false;
  }
  virtual void setH(uint8_t val)
  {
    h = val;
  }
  virtual void setS(uint8_t val)
  {
    s = val;
  }
  virtual void setV(uint8_t val)
  {
    v = val;
  }
  void setHSV(uint8_t h1, uint8_t s1, uint8_t v1)
  {
    h = h1;
    s = s1;
    v = v1;
  }
  void clear_all()
  {
    for (uint8_t i = 0; i < NUM_LEDS; i++)
    {
      global_set_hsv(i, 0, 0, 0);
    }
  }
};
Animation *anim;
class StaticAnimation : public Animation
{
public:
  uint16_t i;

  StaticAnimation(uint8_t frames)
      : Animation(frames)
  {
    i = 0;
    h = 124;
    s = 200;
    v = 135;
  }
  void tick()
  {
    updateAll();
  }
  void updateAll(void)
  {
    for (uint8_t j = 0; j < NUM_LEDS; j++)
    {
      global_set_hsv(j, h, s, v);
    }
  }
};
StaticAnimation *static_anim;
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
  BlinkingAnimation(uint8_t frames)
      : Animation(frames)
  {
    i = 0;
    h = 170;
    s = 200;
    v = 250;
    stage = LOW_K;
    speed = 5;
  }
  void tick()
  {
    if(stage == LOW_K){
      for(uint8_t j = 0; j < NUM_LEDS; j++){
        global_set_hsv(j,h,s,0);
      }
    }else if(stage == HIGH_K){
      for(uint8_t j = 0; j < NUM_LEDS; j++){
        global_set_hsv(j,h,s,v);
      }   
    }else if(stage == INCREASE_K){
      for(uint8_t j = 0; j < NUM_LEDS; j++){
        global_set_hsv(j,h,s,my_max(0+i*speed, 255));
      }      
    }else if(stage == DECREASE_K){
      for(uint8_t j = 0; j < NUM_LEDS; j++){
        global_set_hsv(j,h,s,my_min(v-i*speed, 0));
      }      
    }
    if(stage == INCREASE_K){
      if(i*speed > 255){
        stage = HIGH_K;
        i = 0;
      }
    }
    if(stage == DECREASE_K){
      if(v - i*speed <= 0){
        stage = LOW_K;
        i = 0;
      }
    }
    if(stage == HIGH_K){
      if(i*speed > 20){
        stage = DECREASE_K;
        i = 0;
      }
    }
    if(stage == LOW_K){
      if(i*speed > 20){
        stage = INCREASE_K;
        i = 0;
      }
    }    
    i++;
  }
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
  TravelingDotAnimation(uint8_t frames)
      : Animation(frames)
  {
    i = 0;
    h = 100;
    s = 200;
    v = 100;
    stage = START;
  }
  void tick()
  {
    clear_all();
    uint8_t id;
    if (stage == START)
    {
    }
    if (stage == FORWARD)
    {
      for (int j = 0; j <= 20; j++)
      {
        id = i + j - 20;
        if (id < NUM_LEDS && id >= 0)
          ;
        global_set_hsv(id, h, s, my_max(j * 13, 255));
      }
    }
    else if (stage == REVERSE)
    {
      for (int j = 0; j <= 20; j++)
      {
        id = NUM_LEDS + 20 - j - i;
        if (id < NUM_LEDS && id >= 0)
          ;
        global_set_hsv(id, h, s, my_max(j * 13, 255));
      }
    }
    i++;
    if (i == NUM_LEDS - 1)
    {
      stage = stage == REVERSE ? FORWARD : REVERSE;
      i = 0;
    }
  }
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
  MasterLayer(uint8_t frames)
      : Animation(frames)
  {
    i = 0;
    master_switch = false;
    state = OFF;
  }
  void tick()
  {
    if (state == ONing)
    {
      for (uint8_t j = 0; j < NUM_LEDS; j++)
      {
        global_set_rgb(j, blend(CRGB(0, 0, 0), leds[j], my_max(i * 10, 255)));
        if (i * 10 > 255)
          state = ON;
      }
    }
    else if (state == OFFing)
    {
      for (uint8_t j = 0; j < i; j++)
      {
        global_set_hsv(NUM_LEDS - j, 0, 0, 0);
        global_set_hsv(j, 0, 0, 0);
        if (i == NUM_LEDS / 2 + 1)
          state = OFF;
      }
    }
    if (state == OFF)
    {
      for (uint8_t j = 0; j < NUM_LEDS; j++)
      {
        global_set_hsv(j, 0, 0, 0);
      }
    }
    // if(!master_switch){
    // for(uint8_t j = 0; j < NUM_LEDS; j++){
    //   CHSV hsv = leds[i]
    //   global_set_hsv(i, 0, 0, 0);
    // }
    // }
    i++;
  }
  void Off(void)
  {
    master_switch = false;
    state = OFFing;
    i = 0;
  }
  void On(void)
  {
    master_switch = true;
    state = ONing;
    i = 0;
  }
  void Toggle(void)
  {
    if (master_switch)
      this->Off();
    else
      this->On();
  }
};
MasterLayer *master_layer;
class NotificationLayer : public Animation
{
public:
  uint16_t i;
  bool on_switch;
  bool blinker;

  bool muted = true;
  NotificationLayer(uint8_t frames)
      : Animation(frames)
  {
    i = 0;
    on_switch = false;
    blinker = false;
  }
  void tick()
  {
    if (on_switch)
    {
      if (!muted)
      {
        for (uint8_t j = 33; j < 42; j++)
        {
          global_set_hsv(j, 10, 230, 250);
        }
      }
      if (blinker)
      {
        blink(i, CRGB(50, 250, 10), 5, 65, 75);
      }
    }

    i++;
  }
  void mute()
  {
    muted = true;
  }
  void unmute()
  {
    muted = false;
  }
  void mute_toggle()
  {
    muted = muted ? false : true;
  }
  void on()
  {
    on_switch = true;
  }
  void off()
  {
    on_switch = false;
  }
  void toggle()
  {
    on_switch = on_switch ? false : true;
  }
  void blink(uint16_t i, CRGB color, uint8_t speed, uint8_t start, uint8_t stop)
  {
    uint16_t threshold = ((i)*speed) % 512;
    if (threshold >= 256)
    {
      threshold = 511 - threshold;
    }
    Serial.print(threshold);
    Serial.println("");
    for (int j = start; j <= stop; j++)
    {
      global_set_rgb(j, blend(color, leds[j], threshold));
    }
  }
  void blink_on()
  {
    blinker = true;
  }
  void blink_off()
  {
    blinker = false;
  }
  void blink_toggle()
  {
    blinker = blinker ? false : true;
  }
};
NotificationLayer *notify_layer;
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
  StartAnimation(uint8_t frames)
      : Animation(frames)
  {
    h = 96;
    s = 200;
    v = 250;
    stage = LoadingStrip;
    i = 0;
    done_status = false;
    blink_counter = 0;
  }
  void tick()
  {
    if (stage == LoadingStrip)
    { // TODO start animation in for every tick all leds
      for(uint8_t j = 0; j <= i; j++){
        global_set_hsv(my_max(NUM_LEDS / 2 + j, NUM_LEDS-1), h, s, v);
        global_set_hsv(my_min(NUM_LEDS / 2 - j, 0), h, s, v);
      }

      if (i == NUM_LEDS / 2)
      {
        stage = BlinkDown;
        i = 0;
      }
    }
    else if (stage == BlinkDown || stage == BlinkUp)
    {
      if (stage == BlinkDown)
      {
        for (uint8_t j = 0; j < NUM_LEDS; j++)
          global_set_hsv(j, h, s, v - 10 * (i % v));
      }
      else
      {
        for (uint8_t j = 0; j < NUM_LEDS; j++)
          global_set_hsv(j, h, s, 0 + 10 * (i % v));
      }
      if (blink_counter > 2)
      {
        done_status = true;
      }
      if (stage == BlinkDown && 25 == (i % v))
      {
        stage = BlinkUp;
        blink_counter++;
        i = 0;
      }
      else if (stage == BlinkUp && i % v == 25)
      {
        stage = BlinkDown;
        blink_counter++;
        i = 0;
      }
    }

    i++;
  }
  bool done()
  {
    return done_status;
  }
};
class AnimationsManager
{
private:
  Animation *curr;
  Animation *notification;
  Animation *master_layer;
  int curr_index = 0;

public:
  LinkedList<Animation *> *anim_list;
  AnimationsManager(Animation *anim, Animation *noti, Animation *mast)
  {
    anim_list = new LinkedList<Animation *>();
    anim_list->add(anim);
    curr = anim;
    notification = noti;
    master_layer = mast;
  }
  void add(Animation *anim)
  {
    anim_list->add(anim);
  }

  void tick()
  {
    curr->tick();
    if (curr->done())
    {
      //      pcSerial1.println("next");
      next();
    }
    notification->tick();
    master_layer->tick();
    FastLED.show();
  }
  void next()
  {
    curr_index = (curr_index + 1) % anim_list->size();
    curr = anim_list->get(curr_index);
  }
  void previous()
  {
    curr_index = (curr_index) == 0 ? anim_list->size() - 1 : curr_index - 1;
    curr = anim_list->get(curr_index);
  }
  Animation *get_current(void)
  {
    return curr;
  }
};
AnimationsManager *animManager;


class Screen
{
public:
  uint8_t digits[3][4];
  uint8_t prio_counter[3];
  uint8_t prio_counter_enable[3];
  uint8_t it;
  bool master_on;

  Screen()
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
  void display(uint8_t digits[], uint8_t priority = 0)
  {
    if (priority >= 3)
      return; //ERROR
    for (uint8_t i = 0; i < 4; i++)
    {
      this->digits[priority][i] = digits[i];
    }
  }
  void displayAndUpdate(uint8_t digits[], uint8_t priority = 0)
  {
    display(digits, priority);
    updateScreen();
  }
  void clearAllPrirorities()
  {
    for (uint8_t j = 0; j < 3; j++)
    {
      for (uint8_t i = 0; i < 4; i++)
      {
        digits[j][i] = 0x7f;
      }
    }
  }
  void updateScreen()
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
      uint8_t clear_tab[] = {0x7f, 0x7f, 0x7f, 0x7f};
      tm1637.display(clear_tab);
    }
    it++;
  }
  void clearOnePriority(uint8_t prio)
  {
    if (prio >= 3)
      return;
    for (uint8_t i = 0; i < 4; i++)
    {
      digits[prio][i] = 0x7f;
    }
  }
  void off(void)
  {
    master_on = false;
    // this->updateScreen();
  }
  void on(void)
  {
    master_on = true;
    // this->updateScreen();
  }
  void toggle(void)
  {
    master_on = master_on ? false : true;
  }
  void displayForTime(uint8_t digits[], uint8_t time, uint8_t priority = 0)
  {
    for (uint8_t i = 0; i < 4; i++)
    {
      this->digits[priority][i] = digits[i];
    }
    prio_counter[priority] = (time + it) % 256;
    prio_counter_enable[priority] = true;
    // update_screen();
  }
  void displayForTime(uint16_t num, uint8_t time, uint8_t priority = 0)
  {
    uint8_t digits1[4];
    digits1[0] = (num / 1000) % 10;
    digits1[1] = num / 100;
    digits1[2] = (num / 10) % 10;
    digits1[3] = (num % 10);
    displayForTime(digits1, time, priority);
  }
  void display(uint16_t num, uint8_t priority)
  {
    digits[priority][0] = (num / 1000) % 10;
    digits[priority][1] = num / 100;
    digits[priority][2] = (num / 10) % 10;
    digits[priority][3] = (num % 10);
  }

  void displayTemp(float num)
  {
    num *= 10;
    digits[0][0] = (int)num / 100;
    digits[0][1] = ((int)num / 10) % 10;
    digits[0][2] = ((int)num % 10);
    digits[0][3] = 'C';
    // point(true);
  }
    void displayHum(float num)
  {
    num *= 10;
    digits[0][0] = (int)num / 100;
    digits[0][1] = ((int)num / 10) % 10;
    digits[0][2] = ((int)num % 10);
    digits[0][3] = 'H';
    // point(true);
  }
};
Screen *screen;
uint8_t colorArray[3];
//class serial buffer jest potrzebne

void PanelHandler::s1_on()
{
  master_layer->On();
  panel->led_on(0);
}
void PanelHandler::s1_off()
{
  master_layer->Off();
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
  digitalWrite(LAPTOP_FAN, LOW);
  panel->led_on(2);
}
void PanelHandler::s3_off()
{
  digitalWrite(LAPTOP_FAN, HIGH);
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
  notify_layer->on();
  panel->led_on(5);
}
void PanelHandler::s7_off()
{
  notify_layer->off();
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
  animManager->get_current()->setH(animManager->get_current()->h - 2);
  uint8_t val = animManager->get_current()->h;
  screen->displayForTime(val, 40, 1);
}
void PanelHandler::e1_right()
{
  animManager->get_current()->setH(animManager->get_current()->h + 2);
  uint8_t val = animManager->get_current()->h;
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
  animManager->get_current()->setS(animManager->get_current()->s - 5);
  uint8_t val = animManager->get_current()->s;
  screen->displayForTime(val, 40, 1);
}
void PanelHandler::e3_right()
{
  animManager->get_current()->setS(animManager->get_current()->s + 5);
  uint8_t val = animManager->get_current()->s;
  screen->displayForTime(val, 40, 1);
}
void PanelHandler::e3_btn()
{
}
void PanelHandler::e4_left()
{
  animManager->get_current()->setV(animManager->get_current()->v - 5);
  uint8_t val = animManager->get_current()->v;
  screen->displayForTime(val, 40, 1);
}
void PanelHandler::e4_right()
{
  animManager->get_current()->setV(animManager->get_current()->v + 5);
  uint8_t val = animManager->get_current()->v;
  screen->displayForTime(val, 40, 1);
}
void PanelHandler::e4_btn()
{
}
void PanelHandler::btn1()
{
  notify_layer->mute_toggle();
}
void PanelHandler::btn2()
{
  notify_layer->blink_toggle();
}
void PanelHandler::btn3()
{
  animManager->previous();
}
void PanelHandler::btn4()
{
  animManager->next();
}

inline void clear_serial(void)
{
  while (Serial.available() > 0)
    Serial.read();
}
inline void clear_pc_serial(void)
{
  while (pcSerial1.available() > 0)
    pcSerial1.read();
}
char pserial_buffer[20];
uint8_t psbuff_next_idx = 0;
bool pserial_msg_ready = false;
void panel_serial_task()
{

  while (pcSerial1.available() > 0)
  {
    byte rcv = pcSerial1.read();
    if (rcv == READY_TO_SEND)
    {
      clear_pc_serial();
      pcSerial1.write(READY_TO_RECEIVE);
      pcSerial1.flush();
    }
    else
    {
      return;
    }
    byte interrcnt = 0;
    while (rcv != PRECOMMAND)
    {
      if (interrcnt == 16)
      {
        rcv = ERR_CODE;
        break;
      }
      if (pcSerial1.available() > 0)
      {
        rcv = pcSerial1.read();
      }
      delay(1);
      interrcnt++;
    }

    rcv = pcSerial1.read();
#if DEBUG
    Serial.println("panel");
    Serial.print(rcv);
    Serial.println();
    Serial.flush();
#endif

    if (rcv == S1U)
    {
      panel->switch_toggle(0);
    }
    else if (rcv == S2U)
    {
      panel->switch_toggle(1);
    }
    else if (rcv == S3U)
    {
      panel->switch_toggle(2);
    }
    else if (rcv == S4U)
    {
      panel->switch_toggle(3);
    }
    else if (rcv == S5U)
    {
      panel->switch_toggle(4);
    }
    else if (rcv == S6U)
    {
      panel->switch_toggle(5);
    }
    else if (rcv == S7U)
    {
      panel->switch_toggle(6);
    }
    else if (rcv == S8U)
    {
      panel->switch_toggle(7);
    }
    else if (rcv == E1L)
    {
      panel->enc_left(0);
    }
    else if (rcv == E1R)
    {
      panel->enc_right(0);
    }
    else if (rcv == E1S)
    {
      panel->enc_btn(0);
    }
    else if (rcv == E2L)
    {
      panel->enc_left(1);
    }
    else if (rcv == E2R)
    {
      panel->enc_right(1);
    }
    else if (rcv == E2S)
    {
      panel->enc_btn(1);
    }
    else if (rcv == E3L)
    {
      panel->enc_left(2);
    }
    else if (rcv == E3R)
    {
      panel->enc_right(2);
    }
    else if (rcv == E2S)
    {
      panel->enc_btn(2);
    }
    else if (rcv == E4L)
    {
      panel->enc_left(3);
    }
    else if (rcv == E4R)
    {
      panel->enc_right(3);
    }
    else if (rcv == E4S)
    {
      panel->enc_btn(3);
    }
    else if (rcv == B1P)
    {
      panel->btn(0);
    }
    else if (rcv == B2P)
    {
      panel->btn(1);
    }
    else if (rcv == B3P)
    {
      panel->btn(2);
    }
    else if (rcv == B4P)
    {
      panel->btn(3);
    }
    else if (rcv == PANEL_READY)
    {
      delay(10);
      panel->get_state();
      
    }
    else if (rcv == SWITCHES_SYNCHRO_BEGIN)
    {
      panel->switches_reset();
      
    }
    else if (rcv == SWITCHES_SYNCHRO_END)
    {
      panel->send_led_state();
      panel->synchronized = true;
      animationThread.enabled = true;
      dhtThread.enabled = true;
      screenThread.enabled = true;
    }
  }
  pcSerial1.flush();
  // clear_pc_serial();
}

char serial_buffer[20];
uint8_t sbuff_next_idx = 0;
bool serial_msg_ready = false;
void serial_task(void)
{
  while (Serial.available() > 0)
  {
    serial_buffer[sbuff_next_idx] = Serial.read();

    if (serial_buffer[sbuff_next_idx] == '\n')
    {
      serial_msg_ready = true;
    }
    sbuff_next_idx++;
    if (sbuff_next_idx == 20)
    {
      sbuff_next_idx = 0;
      serial_msg_ready = true;
    }
  }
  if (serial_msg_ready)
  {
#ifdef DEBUG
    Serial.println("Serial");
    Serial.println(serial_buffer);
#endif

    if (MENU.serial_state == SerialStates::Ready)
    {
      if (serial_buffer[0] == 'c')
      {
        serial_buffer[4] = '\0';
        serial_buffer[8] = '\0';
        serial_buffer[12] = '\0';
        colorArray[0] = atoi(&serial_buffer[1]);
        colorArray[1] = atoi(&serial_buffer[5]);
        colorArray[2] = atoi(&serial_buffer[9]);
        Serial.print(colorArray[0]);
        Serial.print(colorArray[1]);
        Serial.print(colorArray[2]);

        static_anim->setHSV(colorArray[0], colorArray[1], colorArray[2]);
      }
      else if (serial_buffer[0] == 'm')
      {
        notify_layer->mute();
      }
      else if (serial_buffer[0] == 'u')
      {
        notify_layer->unmute();
      }
      else if (serial_buffer[0] == 'S')
      {
        if (serial_buffer[1] == '1' && serial_buffer[2] == 'u')
        {

          if (static_anim->v == 0)
          {
            pcSerial1.write(D1N);
            static_anim->v = 200;
          }
          else
          {
            pcSerial1.write(D1F);
            static_anim->v = 0;
          }
        }
      }
      else if (serial_buffer[0] == 'e' && serial_buffer[1] == '1')
      {
        if (serial_buffer[2] == 'r')
        {
          static_anim->h += 2;
        }
        else if (serial_buffer[2] == 'l')
        {
          static_anim->h -= 2;
        }
      }
      else if (serial_buffer[0] == 'e' && serial_buffer[1] == '3')
      {
        if (serial_buffer[2] == 'r')
        {
          static_anim->s += 5;
        }
        else if (serial_buffer[2] == 'l')
        {
          static_anim->s -= 5;
        }
      }
      else if (serial_buffer[0] == 'e' && serial_buffer[1] == '4')
      {
        if (serial_buffer[2] == 'r')
        {
          static_anim->v += 5;
        }
        else if (serial_buffer[2] == 'l')
        {
          static_anim->v -= 5;
        }
      }
    }
    serial_msg_ready = false;
    sbuff_next_idx = 0;
  }
}

void animation_task(void)
{
  animManager->tick();
}

void synchronization_task(void)
{
  if (panel->synchronized)
  {
    synchronizationThread.enabled = false;
  }
  else
  {
    panel->get_state();

  }
}

void screen_task(void)
{
  screen->updateScreen();
}
//TODO
bool temp = true;
void temp_hum_task(void)
{
  sensors_event_t event;
  if(temp){
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature))
    {
      Serial.println(F("Error reading temperature!"));
    }
    else
    {
      screen->displayTemp(event.temperature);
    }
    temp = false;
  }else{
     dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity))
    {
      Serial.println(F("Error reading temperature!"));
    }
    else
    {
      screen->displayHum(event.relative_humidity);
    }
    temp = true;   
  }

}
void setup()
{
  pinMode(4, INPUT);
  pinMode(3, OUTPUT);
  pinMode(LAPTOP_FAN, OUTPUT);

  screen = new Screen();
  dht.begin();

  sbuff_next_idx = 0;
  Animation *travelling_dot = new TravelingDotAnimation(60);
  Animation *blinking_anim = new BlinkingAnimation(60);
  static_anim = new StaticAnimation(60);
  anim = new StartAnimation(60);
  notify_layer = new NotificationLayer(60);
  master_layer = new MasterLayer(60);
  animManager = new AnimationsManager(anim, notify_layer, master_layer);
  PanelHandler *phandler = new PanelHandler();
  panel = new Panel(phandler);
  animManager->add(static_anim);
  animManager->add(travelling_dot);
  animManager->add(blinking_anim);

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS); // GRB ordering is typical

  FastLED.setCorrection(TypicalLEDStrip);
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(0, 0, 0);
  }
  //leds[0] = CRGB(0, 0, 100);
  FastLED.show();
  Serial.begin(115200);
  Serial.println("master");
  pcSerial1.begin(9600);
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(300);

  dhtThread.onRun(temp_hum_task);
  dhtThread.setInterval(3000);
  screenThread.onRun(screen_task);
  screenThread.setInterval(35);
  synchronizationThread.onRun(synchronization_task);
  synchronizationThread.setInterval(3000);
  animationThread.onRun(animation_task);
  animationThread.setInterval(20);
  animationThread.enabled = false;
  dhtThread.enabled = false;
  screenThread.enabled = false;
  controll.add(&animationThread); // & to pass the pointer to it
  controll.add(&synchronizationThread);
  controll.add(&screenThread);
  controll.add(&dhtThread);
}
void loop()
{
  controll.run();
  panel_serial_task();
  serial_task();
}
