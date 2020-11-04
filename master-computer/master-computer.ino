#include "FastLED.h"
#include <LinkedList.h>
#include <SoftwareSerial.h>
#include "utils.h"
#include <Thread.h>
#include <ThreadController.h>

Thread animationThread = Thread();
Thread synchronizationThread = Thread();
//Thread serial_task = Thread();
//Thread panel_serial_task = Thread();


SoftwareSerial pcSerial1(11, 10); // rx, tx
ThreadController controll = ThreadController();

#define NUM_LEDS 90

#define DATA_PIN 5
#define CLOCK_PIN 13
CRGB leds[NUM_LEDS];
void global_set_hsv(uint8_t i, uint8_t h, uint8_t s, uint8_t v){
  if(i >= NUM_LEDS){
    return;
  }
  if(i < 59){
    leds[i].setHSV((h+6)%256,s%256,v%256);
  }else{
    leds[i].setHSV(h%256,s%256,v%256);
  }
}
int my_max(int i, int j){
    if(i > j){
      return j;
    }else{
      return i;
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
  MenuStates state;
  SerialStates serial_state;
  Menu(){
    state = MenuStates::Starting;
    serial_state = SerialStates::Ready;
  }
  void setState(MenuStates mstate){
    state = mstate;
  }
  void setSerial(SerialStates serial){
    serial_state = serial;
  }
};
Menu MENU = Menu();

#define PANEL_LEDS_NUM 6
#define PANEL_SWITCHES_NUM 8
class PanelHandler{
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
class Panel{
  public:
    PanelHandler *handler;
    bool leds[PANEL_LEDS_NUM];
    bool switches[PANEL_SWITCHES_NUM];
    bool synchronized = false;
    Panel(PanelHandler * handler){
      this->handler = handler;
      for(int i = 0; i < PANEL_LEDS_NUM; i++) this->leds[i] = false;
      for(int i = 0; i < PANEL_SWITCHES_NUM; i++) this->switches[i] = false;
    }
  void led_on(uint8_t num){
      if(num >= PANEL_LEDS_NUM) return; //ERROR
      leds[num] = true;
      switch(num){
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
  void led_off(uint8_t num){
      if(num >= PANEL_LEDS_NUM) return; //ERROR
      leds[num] = false;
      switch(num){
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
  void led_toggle(uint8_t num){
      if(num >= PANEL_LEDS_NUM) return; //ERROR
      if(leds[num]){
        led_off(num);
      }else{
        led_on(num);
      }
    }
  void switch_on(uint8_t num){
    if(num >= PANEL_SWITCHES_NUM) return; //ERROR
    switches[num] = true;
    switch(num){
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
  void switch_off(uint8_t num){
    if(num >= PANEL_SWITCHES_NUM) return; //ERROR
    switches[num] = false;
    switch(num){
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
  void switch_toggle(uint8_t num){
      if(num >= PANEL_LEDS_NUM) return; //ERROR
      if(switches[num]){
        switch_off(num);
      }else{
        switch_on(num);
      }
   }
  void enc_left(uint8_t num){
    switch(num){
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
  void enc_right(uint8_t num){
    switch(num){
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
  void enc_btn(uint8_t num){
    switch(num){
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
  void btn(uint8_t num){
    switch(num){
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
  void send_led_state(){
    for(uint8_t i = 0; i < PANEL_LEDS_NUM; i++){
      if(this->leds[i]){
        pcSerial1.write(D1N+(i*2));
      }else{
        pcSerial1.write(D1F+(i*2));
      }
    }
  }
  void get_state(){
    pcSerial1.write(GET_SWITCHES_STATE);
  }
  void switches_reset(){
    for(int i =0; i < PANEL_SWITCHES_NUM; i++){
      switch_off(i);
    }
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
  Animation(uint8_t frames_per_sec){
    fps = frames_per_sec;
    h = 100;
    s = 100;
    v = 100;
  }
  virtual void tick(){
    
  }
  virtual bool done(){
    return false;
  }
  virtual void setH(uint8_t val){
    h = val;
  }
  virtual void setS(uint8_t val){
    s = val;
  }
  virtual void setV(uint8_t val){
    v = val;
  }
  void setHSV(uint8_t h1, uint8_t s1, uint8_t v1){
    h = h1;
    s = s1;
    v = v1;
  }
  void clear_all(){
    for(uint8_t i = 0; i < NUM_LEDS; i++){
      global_set_hsv(i, 0, 0, 0);
    }
  }
};
class StaticAnimation: public Animation
{
  public:
  uint16_t i;

  StaticAnimation(uint8_t frames)
    : Animation(frames)
    {
      i = 0;
      h = 100;
      s = 100;
      v = 100;
  }  
  void tick(){
    updateAll();

  }
  void updateAll(void){
    for(uint8_t j = 0; j < NUM_LEDS; j++){
      global_set_hsv(j, h,s,v);
    }
  }
};
class TravelingDotAnimation: public Animation
{
  public:
  uint16_t i;
  enum stages{
    START, FORWARD, REVERSE
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
  void tick(){
      clear_all();
      uint8_t id;
      if(stage == START){

      }
      if(stage == FORWARD){
        for(int j = 0; j <= 20; j++){
          id = i + j - 20;
          if(id < NUM_LEDS && id >= 0);
          global_set_hsv(id, h,s, my_max(j*13, 255));
        }
      }else if(stage == REVERSE){
        for(int j = 0; j <= 20; j++){
          id = NUM_LEDS + 20 - j - i;
          if(id < NUM_LEDS && id >= 0);
          global_set_hsv(id, h,s, my_max(j*13, 255));
        }
      }
      i++;
      if(i == NUM_LEDS-1){
        stage = stage == REVERSE ? FORWARD: REVERSE;
        i = 0;
      }
  }

};
class MasterLayer: public Animation
{
  public:
  uint16_t i;
  bool master_switch;

  bool muted = true;
  MasterLayer(uint8_t frames)
    : Animation(frames)
    {
      i = 0;
      master_switch = false;

  }  
  void tick(){
    if(!master_switch){
      for(int i = 0; i < NUM_LEDS; i++){
        global_set_hsv(i, 0, 0, 0);
      }
    }
  }
  void Off(void){
    master_switch = false;
  }
  void On(void){
    master_switch = true;
  }
  void Toggle(void){
    master_switch = master_switch ? false : true;
  }
};
class NotificationLayer: public Animation
{
  public:
  uint16_t i;
  bool on_switch;

  bool muted = true;
  NotificationLayer(uint8_t frames)
    : Animation(frames)
    {
      i = 0;
      on_switch = true;
  }  
  void tick(){
    if(on_switch){
      if(!muted){
        for(uint8_t j = 33; j < 42; j++){
          global_set_hsv(j, 10, 230, 250);
        
        }     
      }
    }
  }
  void mute(){
    muted = true;

  }
  void unmute(){
    muted = false;

  }
  void on(){
    on_switch = true;
  }
  void off(){
    on_switch = false;
  }
  void toggle(){
    on_switch = on_switch ? false : true;
  }
};
class StartAnimation: public Animation
{
  private:
    bool done_status;

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
      h = 96;
      s = 200;
      v = 250;
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
       for(uint8_t j = 0; j < NUM_LEDS; j++) global_set_hsv(j,h,s,v-10*(i%v));
     }else{
       for(uint8_t j = 0; j < NUM_LEDS; j++) global_set_hsv(j,h,s,0 + 10*(i%v));
     }
     if(blink_counter > 2){
      done_status = true;
     }
     if(stage == BlinkDown && 25 == (i%v)){
       stage = BlinkUp;
       blink_counter++;
       i = 0;
     }else if( stage == BlinkUp && i%v == 25){
       stage = BlinkDown;
       blink_counter++;
       i = 0;
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
    Animation *master_layer;
    int curr_index = 0;
  public:
    LinkedList<Animation*> *anim_list;
  AnimationsManager(Animation *anim, Animation *noti, Animation *mast){
    anim_list = new LinkedList<Animation*>();
    anim_list->add(anim);
    curr = anim;
    notification = noti;
    master_layer = mast;
  }
  void add(Animation *anim){
    anim_list->add(anim);
  }

  void tick(){
    curr->tick();
    if(curr->done()){
//      pcSerial1.println("next");
      next();
    }
    notification->tick();
    master_layer->tick();
    FastLED.show();
  }
  void next(){
    curr_index = (curr_index+1)%anim_list->size();
    curr = anim_list->get(curr_index);
  }
  void previous(){
    curr_index = (curr_index) == 0 ?  anim_list->size()-1 : curr_index-1;
    curr = anim_list->get(curr_index);
  }
  Animation * get_current(void){
    return curr;
  }
};


#define DEBUG 1

uint8_t colorArray[3];
Animation *anim;
AnimationsManager *animManager;
StaticAnimation *static_anim;
NotificationLayer *notify_layer;
MasterLayer * master_layer;
//class serial buffer jest potrzebne

void PanelHandler::s1_on(){
      master_layer->On();
      panel->led_on(0);
  }
  void PanelHandler::s1_off(){
      master_layer->Off();
      panel->led_off(0);
  }
  void PanelHandler::s1_toggle(){
    
  }
  void PanelHandler::s2_on(){

  }
  void PanelHandler::s2_off(){
    
  }
  void PanelHandler::s2_toggle(){
    
  }
  void PanelHandler::s3_on(){

  }
  void PanelHandler::s3_off(){
    
  }
  void PanelHandler::s3_toggle(){
    
  }
  void PanelHandler::s4_on(){

  }
  void PanelHandler::s4_off(){
    
  }
  void PanelHandler::s4_toggle(){
    
  }
  void PanelHandler::s5_on(){

  }
  void PanelHandler::s5_off(){
    
  }
  void PanelHandler::s5_toggle(){
    
  }
  void PanelHandler::s6_on(){

  }
  void PanelHandler::s6_off(){
    
  }
  void PanelHandler::s6_toggle(){
    
  }
  void PanelHandler::s7_on(){

  }
  void PanelHandler::s7_off(){
    
  }
  void PanelHandler::s7_toggle(){
    
  }
  void PanelHandler::s8_on(){

  }
  void PanelHandler::s8_off(){
    
  }
  void PanelHandler::s8_toggle(){
    
  }
  void PanelHandler::e1_left(){

  }
  void PanelHandler::e1_right(){

  }
  void PanelHandler::e1_btn(){

  }
  void PanelHandler::e2_left(){

  }
  void PanelHandler::e2_right(){

  }
  void PanelHandler::e2_btn(){
    
  }
  void PanelHandler::e3_left(){

  }
  void PanelHandler::e3_right(){

  }
  void PanelHandler::e3_btn(){
    
  }
  void PanelHandler::e4_left(){

  }
  void PanelHandler::e4_right(){

  }
  void PanelHandler::e4_btn(){
    
  }
  void PanelHandler::btn1(){

  }
  void PanelHandler::btn2(){
    
  }
  void PanelHandler::btn3(){
    
  }
  void PanelHandler::btn4(){
    
  }

inline void clear_serial(void){
  while(Serial.available() > 0) Serial.read();
}
inline void clear_pc_serial(void){
  while(pcSerial1.available() > 0) pcSerial1.read();
}

char pserial_buffer[20];
uint8_t psbuff_next_idx = 0;
bool pserial_msg_ready = false;
void panel_serial_task(){

  while(pcSerial1.available() > 0){
    byte rcv = pcSerial1.read();
    if(rcv == READY_TO_SEND){
      clear_pc_serial();
      pcSerial1.write(READY_TO_RECEIVE);
      pcSerial1.flush();
    }else{
      return;
    }
    byte interrcnt = 0;
    while(rcv != PRECOMMAND){
      if(interrcnt == 16){
        rcv = ERR_CODE;
        break;
      }
      if(pcSerial1.available() > 0){
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

      if(rcv == S1U){
        panel->switch_toggle(0);
      }else if(rcv == E1L){
          animManager->get_current()->setH(animManager->get_current()->h-2);
      }else if(rcv == E1R){
          animManager->get_current()->setH(animManager->get_current()->h+2);
      }else if(rcv == E3L){
          animManager->get_current()->setS(animManager->get_current()->s-5);
      }else if(rcv == E3R){
          animManager->get_current()->setS(animManager->get_current()->s+5);
      }else if(rcv == E4L){
          animManager->get_current()->setV(animManager->get_current()->v-5);
      }else if(rcv == E4R){
          animManager->get_current()->setV(animManager->get_current()->v+5);
      }else if(rcv == B1P){
        if(notify_layer->muted){
          notify_layer->unmute();
        }else{
          notify_layer->mute();
        }
      }else if(rcv == B4P){
        animManager->next();
      }else if(rcv == B3P){
        animManager->previous();
      }else if(rcv == S7U){
        notify_layer->toggle();
        if(notify_layer->on_switch) {
          pcSerial1.write(D6N);
        }else{
          pcSerial1.write(D6F);
        }
      }else if(rcv == PANEL_READY){
          delay(10);
          panel->get_state();
          panel->send_led_state();
          panel->synchronized = true;
          animationThread.enabled = true;
      }else if(rcv == SWITCHES_SYNCHRO_BEGIN){
        panel->switches_reset();
      }
    
  }
  pcSerial1.flush();
  clear_pc_serial();
}

char serial_buffer[20];
uint8_t sbuff_next_idx = 0;
bool serial_msg_ready = false;
void serial_task(void){
    while(Serial.available() > 0){
      serial_buffer[sbuff_next_idx] = Serial.read();
      
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
      #ifdef DEBUG
      Serial.println("Serial");
      Serial.println(serial_buffer);
      #endif

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
        }else if(serial_buffer[0] == 'S'){
          if(serial_buffer[1] == '1' && serial_buffer[2] == 'u'){
            
            if(static_anim->v == 0) {
              pcSerial1.write(D1N);
              static_anim->v = 200;
            }
            else{
              pcSerial1.write(D1F);
              static_anim->v = 0;
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

}

void animation_task(void){
  animManager->tick();

}

void synchronization_task(void){
  if(panel->synchronized){
    synchronizationThread.enabled = false;
  }else{
    panel->get_state();
    panel->send_led_state();
    panel->synchronized = true;
    animationThread.enabled = true;
  }
}
void setup() {
    pinMode(4, INPUT);
    pinMode(3, OUTPUT);
    sbuff_next_idx = 0;
    Animation* travelling_dot = new TravelingDotAnimation(60);
    static_anim =  new StaticAnimation(60);
    anim = new StartAnimation(60);
    notify_layer = new NotificationLayer(60);
    master_layer = new MasterLayer(60);
    animManager = new AnimationsManager(anim, notify_layer, master_layer);
    PanelHandler * phandler = new PanelHandler();
    panel = new Panel(phandler);
    animManager->add(static_anim);
    animManager->add(travelling_dot);


    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);  // GRB ordering is typical

    FastLED.setCorrection(TypicalLEDStrip);
    for(int i = 0; i < NUM_LEDS; i++){
      leds[i] = CRGB(0,0,0);
    }
    //leds[0] = CRGB(0, 0, 100);
    FastLED.show();
    Serial.begin(9600);
    Serial.println("master");
    pcSerial1.begin(9600);
    // sanity check delay - allows reprogramming if accidently blowing power w/leds
    delay(500);

    synchronizationThread.onRun(synchronization_task);
    synchronizationThread.setInterval(3000);
    animationThread.onRun(animation_task);
    animationThread.setInterval(20);
    animationThread.enabled = false;
    controll.add(&animationThread); // & to pass the pointer to it
    controll.add(&synchronizationThread);



}
void loop() {
  controll.run();
  panel_serial_task();
  serial_task();
      
}
