#include "FastLED.h"
#include <arduino-timer.h>
#include <cppQueue.h>

Timer<10, millis> timer;

#define NUM_LEDS 120

#define DATA_PIN 5
#define CLOCK_PIN 13
CRGB leds[NUM_LEDS];

enum class MenuStates{
  Starting, Started
};

class Menu
{
  public:
  MenuStates state;
  Menu(void){
    state = MenuStates::Starting;
  }
  void set(MenuStates mstate){
    state = mstate;
  }
  void get(){
    return state;
  }
};
Menu MENU();

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
      h = 60;
      s = 200;
      v = 100;
  }  
  void tick(){

    leds[(NUM_LEDS/2+i)%NUM_LEDS].setHSV(h,s,v);
    leds[NUM_LEDS/2-(i%(NUM_LEDS/2+1))].setHSV(h,s,v);
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
    leds[(NUM_LEDS/2+i)%NUM_LEDS].setHSV(h,s,v);
    leds[NUM_LEDS/2-(i%NUM_LEDS/2)].setHSV(h,s,v);
       if(i == NUM_LEDS/2){
     stage = BlinkDown;
     i = 0;
   }
   }
   else if(stage == BlinkDown || stage == BlinkUp){
     if(stage == BlinkDown){
       for(uint8_t j = 0; j < NUM_LEDS; j++) leds[j].setHSV(h,s,v-(i%v));
     }else{
       for(uint8_t j = 0; j < NUM_LEDS; j++) leds[j].setHSV(h,s,0 + (i%v));
     }
     if(blink_counter > 4){
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
  public:
  Queue *qu;
  AnimationsManager(Animation *anim){
    qu = new Queue(sizeof(curr), 10, FIFO);
    curr = anim;
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
bool serial_task(void){
    if(Serial.available() >= 4){
      Serial.println("czytam");
      colorArray[0] = Serial.parseInt();
      colorArray[1] = Serial.parseInt();
      colorArray[2] = Serial.parseInt();

      Serial.read();
      Serial.print(colorArray[0]);
      Serial.print(colorArray[1]);
      Serial.print(colorArray[2]);
      Serial.println("przeczytalem");
      Serial.print(Serial.available());
      static_anim->setHSV(colorArray[0], colorArray[1], colorArray[2]);

}
  return true;

}

bool animation_task(void){
  animManager->tick();
  return true;
}
void setup() {
    static_anim =  new StaticAnimation(60);
    anim = new StartAnimation(60);
    animManager = new AnimationsManager(anim);
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
