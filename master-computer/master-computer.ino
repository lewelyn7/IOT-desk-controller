#include "FastLED.h"
#include <arduino-timer.h>
#include <cppQueue.h>
#include <SoftwareSerial.h> 
#include "utils.h"
SoftwareSerial pcSerial1(4, 3); // rx, tx
Timer<10, millis> timer;

#define NUM_LEDS 90

#define DATA_PIN 5
#define CLOCK_PIN 13
CRGB leds[NUM_LEDS];
void global_set_hsv(uint8_t i, uint8_t h, uint8_t s, uint8_t v){
  if(i < 59){
    leds[i].setHSV((h+6)%256,s%256,v%256);
  }else{
    leds[i].setHSV(h%256,s%256,v%256);
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
    for(uint8_t j = 0; j < NUM_LEDS; j++){
      global_set_hsv(j, h,s,v);
    }
    if(!i%20){
     // panelSerial.println("d3f");
    }else if(!i%10){
     // panelSerial.println("d3n");
    }
    i++;
  }
  void setHSV(uint8_t &h1, uint8_t &s1, uint8_t &v1){
    h = h1;
    s = s1;
    v = v1;
  }
  void setV(uint8_t &v1){
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
  uint8_t r;
  uint8_t g;
  uint8_t b;

  bool muted = true;
  bool switched = false;
  NotificationLayer(uint8_t frames)
    : Animation(frames)
    {
      i = 0;

  }  
  void tick(){

    if(muted && switched){
      for(uint8_t j = 38; j < 43; j++){
        leds[j].r = r;
        leds[j].g = g;
        leds[j].b = b;
      }

      switched = false;
    }else if(muted == false){
      r = leds[37].r;
      g = leds[37].g;
      b = leds[37].b;
      for(uint8_t j = 38; j < 43; j++){
        global_set_hsv(j, 10, 200, 200);
        
      }
      switched = false;
  
    }
    i++;
  }
  void mute(){
    muted = true;
    switched = true;
  }
  void unmute(){
    muted = false;
    switched = true;
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
//      Serial.println("next");
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

char pserial_buffer[20];
uint8_t psbuff_next_idx;
bool pserial_msg_ready = false;

bool panel_serial_task(){
  while(Serial.available() > 0){
    byte rcv = Serial.read();
    pcSerial1.println("panel");
    pcSerial1.write(rcv);
    pcSerial1.println();
    if(MENU.serial_state == SerialStates::Ready){
      if(rcv == S1U){
        if(static_anim->v == 0) {
          static_anim->v = 200;
          Serial.write(D1N);
        }
        else{
          static_anim->v = 0;
          Serial.write(D1F);
        }          
      }else if(rcv == E1L){
        static_anim->h-=2;
      }else if(rcv == E1R){
        static_anim->h+=2;
      }else if(rcv == E3L){
        static_anim->s+=5;
      }else if(rcv == E3R){
        static_anim->s-=5;
      }else if(rcv == E4L){
        static_anim->v-=5;
      }else if(rcv == E4R){
        static_anim->v+=5;
      }
    }
  }
  Serial.flush();
  return true;
}

//bool panel_serial_task(void){
//  if(panelSerial.available() > 0){
//    byte rcv = panelSerial.read();
//    if(rcv == READY_TO_SEND){
//      panelSerial.write(READY_TO_RECEIVE);
//      
//      do{
//        uint8_t err_cnt = 0;
//        while(panelSerial.available() == 0){
//          delay(5);
//          err_cnt++;
//          if(err_cnt == 5){
//            //ERROR
//            psbuff_next_idx = 0;
//            return true;
//          }
//        }
//        rcv = panelSerial.read();
//        pserial_buffer[psbuff_next_idx] = rcv;
//        psbuff_next_idx++;
//        if(psbuff_next_idx == 20){
//          psbuff_next_idx = 0;
//          //ERROR
//          return true;
//        }
//      }while(rcv != '\n');
//      pserial_msg_ready = true;
//    }
//    }
//    if(pserial_msg_ready){
//    if(MENU.serial_state == SerialStates::Ready){
//    if(pserial_buffer[0] == 'S'){
//      if(pserial_buffer[1] == '1' && pserial_buffer[2] == 'u'){
//        if(static_anim->v == 0) {
//          static_anim->v = 200;
//          panelSerial.println("d1n");
//        }
//        else{
//          static_anim->v = 0;
//          panelSerial.println("d1f");
//        }
//      }
//    }
//    else if(pserial_buffer[0] == 'e' && pserial_buffer[1] == '1'){
//      if(pserial_buffer[2] == 'r'){
//        static_anim->h+=2;
//      }else if(pserial_buffer[2] == 'l'){
//        static_anim->h-=2;
//      }
//    }else if(pserial_buffer[0] == 'e' && pserial_buffer[1] == '3'){
//      if(pserial_buffer[2] == 'r'){
//        static_anim->s+=5;
//      }else if(pserial_buffer[2] == 'l'){
//        static_anim->s-=5;
//      }
//    }else if(pserial_buffer[0] == 'e' && pserial_buffer[1] == '4'){
//      if(pserial_buffer[2] == 'r'){
//        static_anim->v+=5;
//      }else if(pserial_buffer[2] == 'l'){
//        static_anim->v-=5;
//      }
//    }
//  }
//}
//
//  psbuff_next_idx = 0;
//  return true;
//}

//bool panel_serial_task(void){
//    while(panelSerial.available() > 0){
//      pserial_buffer[psbuff_next_idx] = panelSerial.read();
//      
////      Serial.print('p');
////      Serial.print(pserial_buffer[psbuff_next_idx]);
////      Serial.print('\n');
//      if(pserial_buffer[psbuff_next_idx] == '\n'){
//        pserial_msg_ready = true;
//        pserial_buffer[psbuff_next_idx+1] = '\0';
//      }
//      psbuff_next_idx++;
//      if(psbuff_next_idx == 20){
//        psbuff_next_idx = 0;
//        pserial_msg_ready = true;
//      }
//      
//    }
//    if(pserial_msg_ready){
//      Serial.println("pSerial");
//      Serial.println(pserial_buffer);
//      if(MENU.serial_state == SerialStates::Ready){
//        if(pserial_buffer[0] == 'S'){
//          if(pserial_buffer[1] == '1' && pserial_buffer[2] == 'u'){
//            if(static_anim->v == 0) {
//              static_anim->v = 200;
//              panelSerial.println("d1n");
//            }
//            else{
//              static_anim->v = 0;
//              panelSerial.println("d1f");
//            }
//          }
//        }
//        else if(pserial_buffer[0] == 'e' && pserial_buffer[1] == '1'){
//          if(pserial_buffer[2] == 'r'){
//            static_anim->h+=2;
//          }else if(pserial_buffer[2] == 'l'){
//            static_anim->h-=2;
//          }
//        }else if(pserial_buffer[0] == 'e' && pserial_buffer[1] == '3'){
//          if(pserial_buffer[2] == 'r'){
//            static_anim->s+=5;
//          }else if(pserial_buffer[2] == 'l'){
//            static_anim->s-=5;
//          }
//        }else if(pserial_buffer[0] == 'e' && pserial_buffer[1] == '4'){
//          if(pserial_buffer[2] == 'r'){
//            static_anim->v+=5;
//          }else if(pserial_buffer[2] == 'l'){
//            static_anim->v-=5;
//          }
//        }
//      }
//      pserial_msg_ready = false;
//      psbuff_next_idx = 0;
//}
//  return true;
//}

char serial_buffer[20];
uint8_t sbuff_next_idx;
bool serial_msg_ready = false;
bool serial_task(void){
    while(pcSerial1.available() > 0){
      serial_buffer[sbuff_next_idx] = pcSerial1.read();

//      pcSerial1.print('s');
//      pcSerial1.print(serial_buffer[sbuff_next_idx]);
//      pcSerial1.print('\n');
      
      if(serial_buffer[sbuff_next_idx] == '\n'){
        serial_msg_ready = true;
      }
      sbuff_next_idx++;
      if(sbuff_next_idx == 20){
        sbuff_next_idx = 0;
        serial_msg_ready = true;
      }
      
    }
    if(serial_msg_ready){
      pcSerial1.println("Serial");
      pcSerial1.println(serial_buffer);
      if(MENU.serial_state == SerialStates::Ready){
        if(serial_buffer[0] == 'c'){
          serial_buffer[4] = '\0';
          serial_buffer[8] = '\0';
          serial_buffer[12] = '\0';
          colorArray[0] = atoi(&serial_buffer[1]);
          colorArray[1] = atoi(&serial_buffer[5]);
          colorArray[2] = atoi(&serial_buffer[9]);
          pcSerial1.print(colorArray[0]);
          pcSerial1.print(colorArray[1]);
          pcSerial1.print(colorArray[2]);
          
          static_anim->setHSV(colorArray[0], colorArray[1], colorArray[2]);        
          
        }else if(serial_buffer[0] == 'm'){
         notify_layer->mute();
        }else if(serial_buffer[0] == 'u'){
          notify_layer->unmute();
        }else if(serial_buffer[0] == 'S'){
          if(serial_buffer[1] == '1' && serial_buffer[2] == 'u'){
            
            if(static_anim->v == 0) {
              static_anim->v = 200;
              Serial.write(D1N);
            }
            else{
              static_anim->v = 0;
              Serial.println(D1F);
            }
          }
        }
        else if(serial_buffer[0] == 'e' && serial_buffer[1] == '1'){
          if(serial_buffer[2] == 'r'){
            static_anim->h+=2;
          }else if(serial_buffer[2] == 'l'){
            static_anim->h-=2;
          }
        }else if(serial_buffer[0] == 'e' && serial_buffer[1] == '3'){
          if(serial_buffer[2] == 'r'){
            static_anim->s+=5;
          }else if(serial_buffer[2] == 'l'){
            static_anim->s-=5;
          }
        }else if(serial_buffer[0] == 'e' && serial_buffer[1] == '4'){
          if(serial_buffer[2] == 'r'){
            static_anim->v+=5;
          }else if(serial_buffer[2] == 'l'){
            static_anim->v-=5;
          }
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


    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);  // GRB ordering is typical

    FastLED.setCorrection(TypicalLEDStrip);
    for(int i = 0; i < NUM_LEDS; i++){
      leds[i] = CRGB(0,0,0);
    }
    leds[0] = CRGB(0, 0, 100);
    FastLED.show();
    pcSerial1.begin(9600);
    pcSerial1.println("master");
    Serial.begin(9600);
    // sanity check delay - allows reprogramming if accidently blowing power w/leds
    delay(2000);
    Serial.write(D1F);
    Serial.write(D2F);
    Serial.write(D3F);
    Serial.write(D4F);
    Serial.write(D5F);
    Serial.write(D6F);


    timer.every(15, animation_task);
//    timer.every(10, panel_serial_task);
    timer.every(10, serial_task);


}
void loop() {
      timer.tick();
      panel_serial_task();

}
