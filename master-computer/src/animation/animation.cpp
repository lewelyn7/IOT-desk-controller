#include "animation.h"

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

// class Animation
// {
// public:
//   uint8_t fps;
//   uint8_t h;
//   uint8_t s;
//   uint8_t v;
//   CRGB *leds;
//   Animation(CRGB *leds, uint8_t frames_per_sec)
//   {
//     fps = frames_per_sec;
//     h = 100;
//     s = 100;
//     v = 100;
//     this->leds = leds;
//   }
//   virtual void tick()
//   {
//   }
//   virtual bool done()
//   {
//     return false;
//   }
//   virtual void setH(uint8_t val)
//   {
//     h = val;
//   }
//   virtual void setS(uint8_t val)
//   {
//     s = val;
//   }
//   virtual void setV(uint8_t val)
//   {
//     v = val;
//   }
//   void setHSV(uint8_t h1, uint8_t s1, uint8_t v1)
//   {
//     h = h1;
//     s = s1;
//     v = v1;
//   }
//   void clear_all()
//   {
//     for (uint8_t i = 0; i < NUM_LEDS; i++)
//     {
//       global_set_hsv(leds, i, 0, 0, 0);
//     }
//   }
// };
// class StaticAnimation : public Animation
// {
// public:
//   uint16_t i;

//   StaticAnimation(CRGB *leds, uint8_t frames)
//       : Animation(leds, frames)
//   {
//     i = 0;
//     h = 124;
//     s = 200;
//     v = 135;
//   }
//   void tick()
//   {
//     updateAll();
//   }
//   void updateAll(void)
//   {
//     for (uint8_t j = 0; j < NUM_LEDS; j++)
//     {
//       global_set_hsv(leds, j, h, s, v);
//     }
//   }
// };
// class BlinkingAnimation : public Animation
// {
// public:
//   uint16_t i;
//   uint8_t speed;
//   enum stages
//   {
//     INCREASE_K,
//     DECREASE_K,
//     LOW_K,
//     HIGH_K
//   };
//   stages stage;
//   BlinkingAnimation(CRGB *leds, uint8_t frames)
//       : Animation(leds, frames)
//   {
//     i = 0;
//     h = 170;
//     s = 200;
//     v = 250;
//     stage = LOW_K;
//     speed = 5;
//   }
//   void tick()
//   {
//     if(stage == LOW_K){
//       for(uint8_t j = 0; j < NUM_LEDS; j++){
//         global_set_hsv(leds, j,h,s,0);
//       }
//     }else if(stage == HIGH_K){
//       for(uint8_t j = 0; j < NUM_LEDS; j++){
//         global_set_hsv(leds, j,h,s,v);
//       }   
//     }else if(stage == INCREASE_K){
//       for(uint8_t j = 0; j < NUM_LEDS; j++){
//         global_set_hsv(leds, j,h,s,my_max(0+i*speed, 255));
//       }      
//     }else if(stage == DECREASE_K){
//       for(uint8_t j = 0; j < NUM_LEDS; j++){
//         global_set_hsv(leds, j,h,s,my_min(v-i*speed, 0));
//       }      
//     }
//     if(stage == INCREASE_K){
//       if(i*speed > 255){
//         stage = HIGH_K;
//         i = 0;
//       }
//     }
//     if(stage == DECREASE_K){
//       if(v - i*speed <= 0){
//         stage = LOW_K;
//         i = 0;
//       }
//     }
//     if(stage == HIGH_K){
//       if(i*speed > 20){
//         stage = DECREASE_K;
//         i = 0;
//       }
//     }
//     if(stage == LOW_K){
//       if(i*speed > 20){
//         stage = INCREASE_K;
//         i = 0;
//       }
//     }    
//     i++;
//   }
// };
// class TravelingDotAnimation : public Animation
// {
// public:
//   uint16_t i;
//   enum stages
//   {
//     START,
//     FORWARD,
//     REVERSE
//   };
//   stages stage;
//   TravelingDotAnimation(CRGB *leds, uint8_t frames)
//       : Animation(leds, frames)
//   {
//     i = 0;
//     h = 100;
//     s = 200;
//     v = 100;
//     stage = START;
//   }
//   void tick()
//   {
//     clear_all();
//     uint8_t id;
//     if (stage == START)
//     {
//     }
//     if (stage == FORWARD)
//     {
//       for (int j = 0; j <= 20; j++)
//       {
//         id = i + j - 20;
//         if (id < NUM_LEDS && id >= 0)
//           ;
//         global_set_hsv(leds, id, h, s, my_max(j * 13, 255));
//       }
//     }
//     else if (stage == REVERSE)
//     {
//       for (int j = 0; j <= 20; j++)
//       {
//         id = NUM_LEDS + 20 - j - i;
//         if (id < NUM_LEDS && id >= 0)
//           ;
//         global_set_hsv(leds, id, h, s, my_max(j * 13, 255));
//       }
//     }
//     i++;
//     if (i == NUM_LEDS - 1)
//     {
//       stage = stage == REVERSE ? FORWARD : REVERSE;
//       i = 0;
//     }
//   }
// };
// class MasterLayer : public Animation
// {
// public:
//   uint16_t i;
//   bool master_switch;

//   bool muted = true;
//   enum states
//   {
//     OFFing,
//     OFF,
//     ONing,
//     ON
//   };
//   states state;
//   MasterLayer(CRGB *leds, uint8_t frames)
//       : Animation(leds, frames)
//   {
//     i = 0;
//     master_switch = false;
//     state = OFF;
//   }
//   void tick()
//   {
//     if (state == ONing)
//     {
//       for (uint8_t j = 0; j < NUM_LEDS; j++)
//       {
//         global_set_rgb(leds, j, blend(CRGB(0, 0, 0), leds[j], my_max(i * 10, 255)));
//         if (i * 10 > 255)
//           state = ON;
//       }
//     }
//     else if (state == OFFing)
//     {
//       for (uint8_t j = 0; j < i; j++)
//       {
//         global_set_hsv(leds, NUM_LEDS - j, 0, 0, 0);
//         global_set_hsv(leds, j, 0, 0, 0);
//         if (i == NUM_LEDS / 2 + 1)
//           state = OFF;
//       }
//     }
//     if (state == OFF)
//     {
//       for (uint8_t j = 0; j < NUM_LEDS; j++)
//       {
//         global_set_hsv(leds, j, 0, 0, 0);
//       }
//     }
//     // if(!master_switch){
//     // for(uint8_t j = 0; j < NUM_LEDS; j++){
//     //   CHSV hsv = leds[i]
//     //   global_set_hsv(i, 0, 0, 0);
//     // }
//     // }
//     i++;
//   }
//   void Off(void)
//   {
//     master_switch = false;
//     state = OFFing;
//     i = 0;
//   }
//   void On(void)
//   {
//     master_switch = true;
//     state = ONing;
//     i = 0;
//   }
//   void Toggle(void)
//   {
//     if (master_switch)
//       this->Off();
//     else
//       this->On();
//   }
// };
// class NotificationLayer : public Animation
// {
// public:
//   uint16_t i;
//   bool on_switch;
//   bool blinker;

//   bool muted = true;
//   NotificationLayer(CRGB *leds, uint8_t frames)
//       : Animation(leds, frames)
//   {
//     i = 0;
//     on_switch = false;
//     blinker = false;
//   }
//   void tick()
//   {
//     if (on_switch)
//     {
//       if (!muted)
//       {
//         for (uint8_t j = 33; j < 42; j++)
//         {
//           global_set_hsv(leds, j, 10, 230, 250);
//         }
//       }
//       if (blinker)
//       {
//         blink(i, CRGB(50, 250, 10), 5, 65, 75);
//       }
//     }

//     i++;
//   }
//   void mute()
//   {
//     muted = true;
//   }
//   void unmute()
//   {
//     muted = false;
//   }
//   void mute_toggle()
//   {
//     muted = muted ? false : true;
//   }
//   void on()
//   {
//     on_switch = true;
//   }
//   void off()
//   {
//     on_switch = false;
//   }
//   void toggle()
//   {
//     on_switch = on_switch ? false : true;
//   }
//   void blink(uint16_t i, CRGB color, uint8_t speed, uint8_t start, uint8_t stop)
//   {
//     uint16_t threshold = ((i)*speed) % 512;
//     if (threshold >= 256)
//     {
//       threshold = 511 - threshold;
//     }
//     Serial.print(threshold);
//     Serial.println("");
//     for (int j = start; j <= stop; j++)
//     {
//       global_set_rgb(leds, j, blend(color, leds[j], threshold));
//     }
//   }
//   void blink_on()
//   {
//     blinker = true;
//   }
//   void blink_off()
//   {
//     blinker = false;
//   }
//   void blink_toggle()
//   {
//     blinker = blinker ? false : true;
//   }
// };
// class StartAnimation : public Animation
// {
// private:
//   bool done_status;

// public:
//   uint16_t i;
//   uint8_t blink_counter;
//   enum stages
//   {
//     LoadingStrip,
//     BlinkDown,
//     BlinkUp,
//     Ready
//   };
//   stages stage;
//   StartAnimation(CRGB *leds, uint8_t frames)
//       : Animation(leds, frames)
//   {
//     h = 96;
//     s = 200;
//     v = 250;
//     stage = LoadingStrip;
//     i = 0;
//     done_status = false;
//     blink_counter = 0;
//   }
//   void tick()
//   {
//     if (stage == LoadingStrip)
//     { // TODO start animation in for every tick all leds
//       for(uint8_t j = 0; j <= i; j++){
//         global_set_hsv(leds, my_max(NUM_LEDS / 2 + j, NUM_LEDS-1), h, s, v);
//         global_set_hsv(leds, my_min(NUM_LEDS / 2 - j, 0), h, s, v);
//       }

//       if (i == NUM_LEDS / 2)
//       {
//         stage = BlinkDown;
//         i = 0;
//       }
//     }
//     else if (stage == BlinkDown || stage == BlinkUp)
//     {
//       if (stage == BlinkDown)
//       {
//         for (uint8_t j = 0; j < NUM_LEDS; j++)
//           global_set_hsv(leds, j, h, s, v - 10 * (i % v));
//       }
//       else
//       {
//         for (uint8_t j = 0; j < NUM_LEDS; j++)
//           global_set_hsv(leds, j, h, s, 0 + 10 * (i % v));
//       }
//       if (blink_counter > 2)
//       {
//         done_status = true;
//       }
//       if (stage == BlinkDown && 25 == (i % v))
//       {
//         stage = BlinkUp;
//         blink_counter++;
//         i = 0;
//       }
//       else if (stage == BlinkUp && i % v == 25)
//       {
//         stage = BlinkDown;
//         blink_counter++;
//         i = 0;
//       }
//     }

//     i++;
//   }
//   bool done()
//   {
//     return done_status;
//   }
// };
// class AnimationsManager
// {
// private:

//   int curr_index = 0;

// public:

//   Animation *curr;
//   StaticAnimation *static_anim;
//   BlinkingAnimation *blinking_anim;
//   TravelingDotAnimation *tavel_dot_anim;
//   StartAnimation * start_anim;
//   NotificationLayer *notify_layer;
//   MasterLayer *master_layer;
//   LinkedList<Animation *> *anim_list;
//   AnimationsManager(CRGB *leds)
//   {
//     static_anim = new StaticAnimation(leds, 60);
//     blinking_anim = new BlinkingAnimation(leds, 60);
//     tavel_dot_anim = new TravelingDotAnimation(leds, 60);
//     start_anim = new StartAnimation(leds, 60);
//     notify_layer = new NotificationLayer(leds,60);
//     master_layer = new MasterLayer(leds, 60);

//     anim_list = new LinkedList<Animation *>();
//     anim_list->add(static_anim);
//     anim_list->add(blinking_anim);
//     anim_list->add(tavel_dot_anim);
//     anim_list->add(start_anim);
//     curr = static_anim;
//   }
//   void add(Animation *anim)
//   {
//     anim_list->add(anim);
//   }

//   void tick()
//   {
//     curr->tick();
//     if (curr->done())
//     {
//       next();
//     }
//     notify_layer->tick();
//     master_layer->tick();
//     FastLED.show();
//   }
//   void next()
//   {
//     curr_index = (curr_index + 1) % anim_list->size();
//     curr = anim_list->get(curr_index);
//   }
//   void previous()
//   {
//     curr_index = (curr_index) == 0 ? anim_list->size() - 1 : curr_index - 1;
//     curr = anim_list->get(curr_index);
//   }
//   Animation *get_current(void)
//   {
//     return curr;
//   }
// };




Animation::Animation(CRGB *leds, uint8_t frames_per_sec) 
{
      fps = frames_per_sec;
    h = 100;
    s = 100;
    v = 100;
    this->leds = leds;
}

void Animation::tick() 
{
     // return false;
}
  bool Animation::done() 
  {
    return false;
  }
void Animation::setH(uint8_t val) 
{
      h = val;

}

void Animation::setS(uint8_t val) 
{
      s = val;

}

void Animation::setV(uint8_t val) 
{
      v = val;

}

void Animation::setHSV(uint8_t h1, uint8_t s1, uint8_t v1) 
{
      h = h1;
    s = s1;
    v = v1;
}

void Animation::clear_all() 
{
      for (uint8_t i = 0; i < NUM_LEDS; i++)
    {
      global_set_hsv(leds, i, 0, 0, 0);
    }
}

StaticAnimation::StaticAnimation(CRGB *leds, uint8_t frames) 
: Animation(leds, frames)
{
     i = 0;
    h = 124;
    s = 200;
    v = 135; 
}

void StaticAnimation::tick() 
{
      updateAll();
}

void StaticAnimation::updateAll(void) 

{
      for (uint8_t j = 0; j < NUM_LEDS; j++)
    {
      global_set_hsv(leds, j, h, s, v);
    }
}

BlinkingAnimation::BlinkingAnimation(CRGB *leds, uint8_t frames) 
: Animation(leds, frames)
{
      i = 0;
    h = 170;
    s = 200;
    v = 250;
    stage = LOW_K;
    speed = 5;
}

void BlinkingAnimation::tick() 
  {
    if(stage == LOW_K){
      for(uint8_t j = 0; j < NUM_LEDS; j++){
        global_set_hsv(leds, j,h,s,0);
      }
    }else if(stage == HIGH_K){
      for(uint8_t j = 0; j < NUM_LEDS; j++){
        global_set_hsv(leds, j,h,s,v);
      }   
    }else if(stage == INCREASE_K){
      for(uint8_t j = 0; j < NUM_LEDS; j++){
        global_set_hsv(leds, j,h,s,my_max(0+i*speed, 255));
      }      
    }else if(stage == DECREASE_K){
      for(uint8_t j = 0; j < NUM_LEDS; j++){
        global_set_hsv(leds, j,h,s,my_min(v-i*speed, 0));
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

TravelingDotAnimation::TravelingDotAnimation(CRGB *leds, uint8_t frames) 
  : Animation(leds, frames)
  {
    i = 0;
    h = 100;
    s = 200;
    v = 100;
    stage = START;
  }

void TravelingDotAnimation::tick() 
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
        global_set_hsv(leds, id, h, s, my_max(j * 13, 255));
      }
    }
    else if (stage == REVERSE)
    {
      for (int j = 0; j <= 20; j++)
      {
        id = NUM_LEDS + 20 - j - i;
        if (id < NUM_LEDS && id >= 0)
          ;
        global_set_hsv(leds, id, h, s, my_max(j * 13, 255));
      }
    }
    i++;
    if (i == NUM_LEDS - 1)
    {
      stage = stage == REVERSE ? FORWARD : REVERSE;
      i = 0;
    }
  }

MasterLayer::MasterLayer(CRGB *leds, uint8_t frames) 
: Animation(leds, frames)
  {
    i = 0;
    master_switch = false;
    state = OFF;
  }

void MasterLayer::tick() 
  {
    if (state == ONing)
    {
      for (uint8_t j = 0; j < NUM_LEDS; j++)
      {
        global_set_rgb(leds, j, blend(CRGB(0, 0, 0), leds[j], my_max(i * 10, 255)));
        if (i * 10 > 255)
          state = ON;
      }
    }
    else if (state == OFFing)
    {
      for (uint8_t j = 0; j < i; j++)
      {
        global_set_hsv(leds, NUM_LEDS - j, 0, 0, 0);
        global_set_hsv(leds, j, 0, 0, 0);
        if (i == NUM_LEDS / 2 + 1)
          state = OFF;
      }
    }
    if (state == OFF)
    {
      for (uint8_t j = 0; j < NUM_LEDS; j++)
      {
        global_set_hsv(leds, j, 0, 0, 0);
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

void MasterLayer::Off(void) 
  {
    master_switch = false;
    state = OFFing;
    i = 0;
  }
void MasterLayer::On(void) 
  {
    master_switch = true;
    state = ONing;
    i = 0;
  }
void MasterLayer::Toggle(void) 
  {
    if (master_switch)
      this->Off();
    else
      this->On();
  }

NotificationLayer::NotificationLayer(CRGB *leds, uint8_t frames) 
: Animation(leds, frames)
  {
    i = 0;
    on_switch = false;
    blinker = false;
  }

void NotificationLayer::tick() 
  {
    if (on_switch)
    {
      if (!muted && !discord_muted)
      {
        for (uint8_t j = 33; j < 42; j++)
        {
          global_set_hsv(leds, j, 10, 230, 250);
        }
      }else if(discord_muted && !muted){
        for (uint8_t j = 33; j < 42; j++)
        {
          global_set_hsv(leds, j, 56, 230, 250);
        }        
      }
      if (blinker)
      {
        blink(i, CRGB(50, 250, 10), 5, 65, 75);
      }
    }

    i++;
  }
void NotificationLayer::mute() 
  {
    muted = true;
    discord_muted = true;
  }
void NotificationLayer::unmute() 
  {
    muted = false;
    discord_muted = false;
  }
  
  void NotificationLayer::discord_mute() 
  {
    discord_muted = true;
  }

void NotificationLayer::mute_toggle() 
  {
    muted = muted ? false : true;
  }

void NotificationLayer::on() 
  {
    on_switch = true;
  }

void NotificationLayer::off() 
  {
    on_switch = false;
  }

void NotificationLayer::toggle() 
  {
    on_switch = on_switch ? false : true;
  }

void NotificationLayer::blink(uint16_t i, CRGB color, uint8_t speed, uint8_t start, uint8_t stop) 
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
      global_set_rgb(leds, j, blend(color, leds[j], threshold));
    }
  }

void NotificationLayer::blink_on() 
  {
    blinker = true;
  }

void NotificationLayer::blink_off() 
  {
    blinker = false;
  }

void NotificationLayer::blink_toggle() 
  {
    blinker = blinker ? false : true;
  }

StartAnimation::StartAnimation(CRGB *leds, uint8_t frames) 
      : Animation(leds, frames)
  {
    h = 96;
    s = 200;
    v = 250;
    stage = LoadingStrip;
    i = 0;
    done_status = false;
    blink_counter = 0;
  }
void StartAnimation::tick() 
  {
    if (stage == LoadingStrip)
    { // TODO start animation in for every tick all leds
      for(uint8_t j = 0; j <= i; j++){
        global_set_hsv(leds, my_max(NUM_LEDS / 2 + j, NUM_LEDS-1), h, s, v);
        global_set_hsv(leds, my_min(NUM_LEDS / 2 - j, 0), h, s, v);
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
          global_set_hsv(leds, j, h, s, v - 10 * (i % v));
      }
      else
      {
        for (uint8_t j = 0; j < NUM_LEDS; j++)
          global_set_hsv(leds, j, h, s, 0 + 10 * (i % v));
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
  


bool StartAnimation::done() 
  {
    return done_status;
  }
AnimationsManager::AnimationsManager(CRGB *leds) 
  {
    static_anim = new StaticAnimation(leds, 60);
    blinking_anim = new BlinkingAnimation(leds, 60);
    tavel_dot_anim = new TravelingDotAnimation(leds, 60);
    start_anim = new StartAnimation(leds, 60);
    notify_layer = new NotificationLayer(leds,60);
    master_layer = new MasterLayer(leds, 60);

    anim_list = new LinkedList<Animation *>();
    anim_list->add(static_anim);
    anim_list->add(blinking_anim);
    anim_list->add(tavel_dot_anim);
    anim_list->add(start_anim);
    curr = static_anim;
  }

void AnimationsManager::add(Animation *anim) 
  {
    anim_list->add(anim);
  }

void AnimationsManager::tick() 
  {
    curr->tick();
    if (curr->done())
    {
      next();
    }
    notify_layer->tick();
    master_layer->tick();
    FastLED.show();
  }

void AnimationsManager::next() 
  {
    curr_index = (curr_index + 1) % anim_list->size();
    curr = anim_list->get(curr_index);
  }
void AnimationsManager::previous() 
  {
    curr_index = (curr_index) == 0 ? anim_list->size() - 1 : curr_index - 1;
    curr = anim_list->get(curr_index);
  }

Animation* AnimationsManager::get_current(void) 
  {
    return curr;
  }
