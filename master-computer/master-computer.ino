
#include <LinkedList.h>
#include "utils.h"
#include <Thread.h>
#include <ThreadController.h>
#include <FastLED.h>
#include <SoftwareSerial.h>
#include "globals.h"
#include "Animation/animation.h"


Thread dhtThread = Thread();
Thread animationThread = Thread();
Thread synchronizationThread = Thread();
Thread screenThread = Thread();
//Thread serial_task = Thread();
//Thread panel_serial_task = Thread();
ThreadController controll = ThreadController();
DHT_Unified dht(DHTPIN, DHTTYPE);



#define PANEL_LEDS_NUM 6
#define PANEL_SWITCHES_NUM 8

Menu MENU = Menu();

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
// class Panel
// {
// public:
//   PanelHandler *handler;
//   bool leds[PANEL_LEDS_NUM];
//   bool switches[PANEL_SWITCHES_NUM];
//   bool synchronized = false;
//   bool synchronization_mode = false;
//   Panel(PanelHandler *handler);

//   void led_on(uint8_t num);

//   void led_off(uint8_t num);

//   void led_toggle(uint8_t num);

//   void switch_on(uint8_t num);

//   void switch_off(uint8_t num);

//   void switch_toggle(uint8_t num);

//   void enc_left(uint8_t num);

//   void enc_right(uint8_t num);

//   void enc_btn(uint8_t num);

//   void btn(uint8_t num);

//   void send_led_state();

//   void get_state();

//   void switches_reset();

// };
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
extern Panel *panel;

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




extern PanelHandler *phandler;
uint8_t colorArray[3];
//class serial buffer jest potrzebne

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
    #if DEBUG
    Serial.println("panel available");
    Serial.print(rcv);
    Serial.println("");
    #endif
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
    bool precommand_ack = false;
    while (rcv == PRECOMMAND || !precommand_ack)
    {
      if (interrcnt == 16)
      {
        rcv = ERR_CODE;
        break;
      }
      if (pcSerial1.available() > 0)
      {
        rcv = pcSerial1.read();
            Serial.print(rcv); //TODO
            Serial.print('\n');
        if(rcv == PRECOMMAND){
          precommand_ack = true;
        }
      }
      delay(1);
      interrcnt++;
    }

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

        animationManager->static_anim->setHSV(colorArray[0], colorArray[1], colorArray[2]);
      }
      else if (serial_buffer[0] == 'm')
      {
        animationManager->notify_layer->mute();
      }
      else if (serial_buffer[0] == 'u')
      {
        animationManager->notify_layer->unmute();
      }
      else if (serial_buffer[0] == 'S')
      {
        if (serial_buffer[1] == '1' && serial_buffer[2] == 'u')
        {

          if (animationManager->static_anim->v == 0)
          {
            pcSerial1.write(D1N);
            animationManager->static_anim->v = 200;
          }
          else
          {
            pcSerial1.write(D1F);
            animationManager->static_anim->v = 0;
          }
        }
      }
      else if (serial_buffer[0] == 'e' && serial_buffer[1] == '1')
      {
        if (serial_buffer[2] == 'r')
        {
          animationManager->static_anim->h += 2;
        }
        else if (serial_buffer[2] == 'l')
        {
          animationManager->static_anim->h -= 2;
        }
      }
      else if (serial_buffer[0] == 'e' && serial_buffer[1] == '3')
      {
        if (serial_buffer[2] == 'r')
        {
          animationManager->static_anim->s += 5;
        }
        else if (serial_buffer[2] == 'l')
        {
          animationManager->static_anim->s -= 5;
        }
      }
      else if (serial_buffer[0] == 'e' && serial_buffer[1] == '4')
      {
        if (serial_buffer[2] == 'r')
        {
          animationManager->static_anim->v += 5;
        }
        else if (serial_buffer[2] == 'l')
        {
          animationManager->static_anim->v -= 5;
        }
      }
    }
    serial_msg_ready = false;
    sbuff_next_idx = 0;
  }
}

void animation_task(void)
{
  animationManager->tick();
}

void synchronization_task(void)
  {
  if (panel->synchronized)
  {
    //TODO 
    Serial.println("PANEL_SYNCHRO");
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



  // pinMode(4, INPUT);
  // pinMode(3, OUTPUT);
  pinMode(LAPTOP_FAN, OUTPUT);

  dht.begin();
  sbuff_next_idx = 0;

  phandler = new PanelHandler();
  panel = new Panel(phandler);
  screen = new Screen();
  animationManager = new AnimationsManager(leds);

  FastLED.addLeds<WS2812B, DATA_PIN_WS, GRB>(leds, NUM_LEDS); // GRB ordering is typical

  FastLED.setCorrection(TypicalLEDStrip);
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(0, 0, 0);
  }
  FastLED.show();
  Serial.begin(115200);
  Serial.println("master");
  pcSerial1.begin(57600, SWSERIAL_8N1, SERIAL_RX_PIN, SERIAL_TX_PIN, false, 64, 64);
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
  #ifdef DEBUG
  Serial.println("setup done\r\n");
  #endif
}
void loop()
{
  controll.run();
  panel_serial_task();
  serial_task();
}
