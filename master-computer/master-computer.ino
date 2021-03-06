
#include <LinkedList.h>
#include <Thread.h>
#include <ThreadController.h>
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "src/globals.h"
#include "src/Menu/Menu.h"
#include "src/LedDisplay/LedDisplay.h"
#include "src/utils/utils.h"
#include "src/animation/animation.h"
#include "src/Panel/Panel.h"
#include "src/MQTTCommunicator/MQTTCommunicator.h"
#include "src/DsClock/DsClock.h"
#include "src/LCDdisplay/LCDdisplay.h"
#include <FastLED.h>

LCDdisplay * lcd;
DsClock *dsclock;
Screen *screen;
Menu * menu;
Panel *panel;
PanelHandler *phandler;
MQTTCommunicator *mqtt;
uint8_t colorArray[3];

CRGB leds[NUM_LEDS];
AnimationsManager *animationManager;
SoftwareSerial pcSerial1; // rx, tx
DHT_Unified dht(DHTPIN, DHTTYPE);

Thread dhtThread = Thread();
Thread animationThread = Thread();
Thread synchronizationThread = Thread();
Thread screenThread = Thread();
Thread mqttConnectionThread = Thread();
Thread mqttHsvStripUpdateThread = Thread();
//Thread serial_task = Thread();
//Thread panel_serial_task = Thread();
ThreadController controll = ThreadController();


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


///////// SERIAL ////////////////
#define MSERIAL_BUFFER_SIZE 21
char serial_buffer[MSERIAL_BUFFER_SIZE];
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
    if (sbuff_next_idx == MSERIAL_BUFFER_SIZE-1)
    {
      sbuff_next_idx = 0;
      serial_msg_ready = true;
    }
  }
  if (serial_msg_ready)
  {
    serial_buffer[sbuff_next_idx] = '\0';
#ifdef DEBUG
    Serial.println("Serial");
    Serial.println(serial_buffer);
#endif

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
      else if (!strncmp(serial_buffer, "m\n", MSERIAL_BUFFER_SIZE))
      {
        animationManager->notify_layer->mute();
      }
      else if (!strncmp(serial_buffer, "u\n", MSERIAL_BUFFER_SIZE))
      {
        animationManager->notify_layer->unmute();
      }
      else if (!strncmp(serial_buffer, "d\n", MSERIAL_BUFFER_SIZE))
      {
        animationManager->notify_layer->discord_mute();
      }   
      else if (!strncmp(serial_buffer, "tsleep\n", MSERIAL_BUFFER_SIZE))
      {
        animationManager->next();
      }   
      else if (!strncmp(serial_buffer, "wakeup\n", MSERIAL_BUFFER_SIZE))
      {
        animationManager->previous();
      }    
      else if (!strncmp(serial_buffer, "init\n", MSERIAL_BUFFER_SIZE))
      {
        animationManager->previous();
        animationManager->set_current(animationManager->start_anim);
      }                        
    serial_msg_ready = false;
    sbuff_next_idx = 0;
  }
}

void animation_task(void)
{
  animationManager->tick();
}


// panel synchronization on startup
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

void mqtt_reconnect_task(void){
  mqtt->mqtt_reconnect();
}

void mqtt_update_strip_hsv_task(void){
  if(!mqtt->hsv_up_to_date){
    mqtt->update_led_strip_hsv();
    mqtt->hsv_up_to_date = true;
  }
  if(!mqtt->led_brig_up_to_date){
    mqtt->update_led_brightness();
    mqtt->led_brig_up_to_date = true;
  }
}


//TODO

void rtc_update_task(void)
{
    dsclock->rtc_update();
    screen->displayTime(dsclock->getHours(), dsclock->getMinutes());
}
void setup()
{

  pinMode(LAPTOP_FAN, OUTPUT);

  Serial.begin(115200);
  Serial.println("INFO::master computer HELLO");

  lcd = new LCDdisplay();
  lcd->setCursor(0,0);
  lcd->printstr("helloooo");
  lcd->setCursor(0,1);
  lcd->printstr("co tam?");

  phandler = new PanelHandler();
  panel = new Panel(phandler);
  screen = new Screen();
  animationManager = new AnimationsManager(leds);
  
  FastLED.addLeds<WS2812B, DATA_PIN_WS, GRB>(leds, NUM_LEDS); // GRB ordering is typical

  FastLED.setCorrection( 0xC8DCFF);
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(0, 0, 0);
  }
  FastLED.show();

  pcSerial1.begin(57600, SWSERIAL_8N1, SERIAL_RX_PIN, SERIAL_TX_PIN, false, 32, 32);
  mqtt = new MQTTCommunicator();
  menu = new Menu();
  dsclock = new DsClock();
  dsclock->update_time();

  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(300);

  dhtThread.onRun(rtc_update_task);
  dhtThread.setInterval(1000);
  screenThread.onRun(screen_task);
  screenThread.setInterval(22);
  mqttConnectionThread.onRun(mqtt_reconnect_task);
  mqttConnectionThread.setInterval(1000);
  mqttHsvStripUpdateThread.onRun(mqtt_update_strip_hsv_task);
  mqttHsvStripUpdateThread.setInterval(100);
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
  controll.add(&mqttConnectionThread);
  controll.add(&mqttHsvStripUpdateThread);




  #ifdef DEBUG
  Serial.println("INFO::setup done");
  #endif
}
void loop()
{
  controll.run();
  panel_serial_task();
  serial_task();
  mqtt->loop();
}
