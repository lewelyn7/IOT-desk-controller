#include "FastLED.h"
#include <arduino-timer.h>
#include <cppQueue.h>

Timer<10, millis> timer;

#define NUM_LEDS 90

#define DATA_PIN 5
#define CLOCK_PIN 13
CRGB leds[NUM_LEDS];
void global_set_hsv(uint8_t i, uint8_t h, uint8_t s, uint8_t v){
  if(i < 59){
    leds[i].setHSV((h+6)%255,s,v);
  }else{
    leds[i].setHSV(h,s,v);
  }
}
enum class MenuStates{
  Starting, Started,
};
enum class SerialStates{
  Ready, ColorReadingHSV
};
class Menu
{
  public:
  MenuStates state = MenuStates::Starting;
  SerialStates serial_state = SerialStates::Ready;
  Menu(){
    state = MenuStates::Starting;
    serial_state = SerialStates::Ready;
  }
  void setState(MenuStates mstate){
    state = mstate;
  }
  void getState(){
    return state;
  }
  void setSerial(SerialStates serial){
    serial_state = serial;
  }
};
Menu MENU;

class Animation
{
  public:
  uint8_t fps;
  Animation(uint8_t frames_per_sec){
    fps = frames_per_sec;
  }
  virtual void tick(){
    
  }
  virtual bool done(){
    return false;
  }
};
class StaticAnimation: public Animation
{
  public:
  uint16_t i;
  uint8_t h;
  uint8_t s;
  uint8_t v;
  StaticAnimation(uint8_t frames)
    : Animation(frames)
    {
      i = 0;
      h = 100;
      s = 100;
      v = 100;
  }  
  void tick(){

    global_set_hsv((NUM_LEDS/2+i)%NUM_LEDS, h,s,v);
    global_set_hsv(NUM_LEDS/2-(i%(NUM_LEDS/2+1)), h,s,v);
    if(i == NUM_LEDS/2){
      i = 0;
    }
    i++;
  }
  void setHSV(uint8_t &h1, uint8_t &s1, uint8_t &v1){
    h = h1;
    s = s1;
    v = v1;
  }
  void mute(){
    global_set_hsv(40, 10, 200, 200);
    global_set_hsv(41, 10, 200, 200);
    global_set_hsv(42, 10, 200, 200);
  }
  void unmute(){
    global_set_hsv(40, h,s,v);
    global_set_hsv(41, h,s,v);
    global_set_hsv(42, h,s,v);

  }
};
class NotificationLayer: public Animation
{
  public:
  uint16_t i;

  bool muted = true;
  NotificationLayer(uint8_t frames)
    : Animation(frames)
    {
      i = 0;

  }  
  void tick(){

    if(muted){
  
    }else{
      for(uint8_t j = 38; j < 43; j++){
        global_set_hsv(j, 10, 200, 200);
        
      }
  
    }
    i++;
  }
  void mute(){
    muted = true;
  }
  void unmute(){
    muted = false;
  }
};
class StartAnimation: public Animation
{
  private:
    bool done_status;
    const uint8_t h = 96;
    const uint8_t s = 200;
    const uint8_t v = 200;
  public:
  uint16_t i;
  uint8_t blink_counter;
  enum stages{
    LoadingStrip, BlinkDown, BlinkUp, Ready
  };
  stages stage;
  StartAnimation(uint8_t frames)
    : Animation(frames)
    {
      stage = LoadingStrip;
      i = 0;
      done_status = false;
      blink_counter = 0;
  }
  void tick(){
   if(stage == LoadingStrip){
    global_set_hsv((NUM_LEDS/2+i)%NUM_LEDS,h,s,v);
    global_set_hsv(NUM_LEDS/2-(i%NUM_LEDS/2),h,s,v);
       if(i == NUM_LEDS/2){
     stage = BlinkDown;
     i = 0;
   }
   }
   else if(stage == BlinkDown || stage == BlinkUp){
     if(stage == BlinkDown){
       for(uint8_t j = 0; j < NUM_LEDS; j++) global_set_hsv(j,h,s,v-(i%v));
     }else{
       for(uint8_t j = 0; j < NUM_LEDS; j++) global_set_hsv(j,h,s,0 + (i%v));
     }
     if(blink_counter > 2){
      done_status = true;
     }
     if(stage == BlinkDown && v - 1 == (i%v)){
       stage = BlinkUp;
       blink_counter++;
     }else if( stage == BlinkUp && i%v == v-1){
       stage = BlinkDown;
       blink_counter++;
     }
   }


   
   i++;
  }
  bool done(){
    return done_status;
  }
};
class AnimationsManager
{
  private:
    Animation *curr;
    Animation *notification;
  public:
  Queue *qu;
  AnimationsManager(Animation *anim, Animation *noti){
    qu = new Queue(sizeof(curr), 10, FIFO);
    curr = anim;
    notification = noti;
  }
  void push(Animation *anim){
    qu->push(&anim);
  }
  void pop(){
    Animation anim();
    qu->pop(&anim);
  }
  void tick(){
    curr->tick();
    if(curr->done()){
      Serial.println("next");
      next();
    }
    notification->tick();
    FastLED.show();
  }
  void next(){
    qu->pop(&curr);
  }
};



uint8_t colorArray[3];
Animation *anim;
AnimationsManager *animManager;
StaticAnimation *static_anim;
NotificationLayer *notify_layer;
//class serial buffer jest potrzebne
char serial_buffer[20];
uint8_t sbuff_next_idx;
bool serial_msg_ready = false;
bool serial_task(void){
    while(Serial.available() > 0){
      serial_buffer[sbuff_next_idx] = Serial.read();
      if(serial_buffer[sbuff_next_idx] == '\n'){
        serial_msg_ready = true;
      }
      sbuff_next_idx++;
      if(sbuff_next_idx == 20){
        sbuff_next_idx = 0;
        sbuff_next_idx = true;
      }
      
    }
    if(serial_msg_ready){
      Serial.println("Serial");
      if(MENU.serial_state == SerialStates::Ready){
        if(serial_buffer[0] == 'c'){
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
          
        }else if(serial_buffer[0] == 'm'){
         notify_layer->mute();
        }else if(serial_buffer[0] == 'u'){
          notify_layer->unmute();
        }
      }
      serial_msg_ready = false;
      sbuff_next_idx = 0;
}
  return true;

}

bool animation_task(void){
  animManager->tick();
  return true;
}
void setup() {
    sbuff_next_idx = 0;
    static_anim =  new StaticAnimation(60);
    anim = new StartAnimation(60);
    notify_layer = new NotificationLayer(60);
    animManager = new AnimationsManager(anim, notify_layer);

    animManager->push(static_anim);
    Serial.begin(9600);
    // sanity check delay - allows reprogramming if accidently blowing power w/leds
    delay(2000);

    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);  // GRB ordering is typical

    FastLED.setCorrection(TypicalLEDStrip);
    for(int i = 0; i < NUM_LEDS; i++){
      leds[i] = CRGB(0,0,0);
    }
    leds[0] = CRGB(0, 0, 100);
    FastLED.show();
    timer.every(20, animation_task);
    timer.every(30, serial_task);
}
void loop() {
      timer.tick();
//    animManager->tick();
//    FastLED.show();
//    delay(20);

}
