#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include "PinChangeInterrupt.h"
#include <string.h>

// Install the LowPower library for optional sleeping support.
// See loop() function comments for details on usage.
//#include <LowPower.h>
// Basic pin reading and pullup test for the MCP23017 I/O expander
// public domain!

// Connect pin #12 of the expander to Analog 5 (i2c clock)
// Connect pin #13 of the expander to Analog 4 (i2c data)
// Connect pins #15, 16 and 17 of the expander to ground (address selection)
// Connect pin #9 of the expander to 5V (power)
// Connect pin #10 of the expander to ground (common ground)
// Connect pin #18 through a ~10kohm resistor to 5V (reset pin, active low)

// Input #0 i
#include <Wire.h>
#include <Adafruit_MCP23017.h>
//s on pin 21 so connect a button or switch from there to ground

#define D1 0
#define D2 2
#define D3 3
#define D4 4
#define D5 5
#define D6 1

#define SM 7

#define S1 12
#define S2 15
#define S3 14//13//14
#define S4 10//9//15
#define S5 11//10//11
#define S6 13
#define S7 8
#define S8 9

#define E1_S 8
#define E1_A A6
#define E1_B 3

#define E2_S 9
#define E2_A A7
#define E2_B 4

#define E3_S 12
#define E3_A 10
#define E3_B 5

#define E4_S 7
#define E4_A 11
#define E4_B 6


#define B1 A0
#define B2 A1
#define B3 A2
#define B4 A3

//NIE UZYWAC INPUT_PULLUP DO PRZERWAN TYPU INT0/1


int E1pinAstateCurrent = LOW;                // Current state of Pin A
int E1pinAStateLast = E1pinAstateCurrent;      // Last read value of Pin A

int E2pinAstateCurrent = LOW;                // Current state of Pin A
int E2pinAStateLast = E2pinAstateCurrent;      // Last read value of Pin A

int E3pinAstateCurrent = LOW;                // Current state of Pin A
int E3pinAStateLast = E3pinAstateCurrent;      // Last read value of Pin A

int E4pinAstateCurrent = LOW;                // Current state of Pin A
int E4pinAStateLast = E4pinAstateCurrent;      // Last read value of Pin A

char * switches[] = { "S7u", "S8u", "S4u", "S5u", "S1u", "S6u", "S3u", "S2u" };


//MCP
Adafruit_MCP23017 mcp;

int led_mapper(int led_num){
  switch(led_num){
    case 1:
      return D1;
    case 2:
      return D2;
    case 3:
      return D3;
    case 4:
      return D4;
    case 5:
      return D5;
    case 6:
      return D6;   
   }
   Serial.println("RRR");
   return 0;
}

void led_on(int num){
  if(num <= 6){
    mcp.digitalWrite(led_mapper(num), HIGH);
  }
  
}
void led_off(int num){
  if(num <= 6){
    mcp.digitalWrite(led_mapper(num), LOW);
  }
  
}

byte ledPin=LED_BUILTIN;
#define SERIAL_RX_PIN 0
// Interrupts from the MCP will be handled by this PIN
byte arduinoIntPin=2;
volatile boolean awakenByInterruptMCP = false;

volatile boolean serial_to_read = false;

volatile boolean button_to_read = false;

volatile boolean encoder_to_send = false;

volatile boolean enc1_switch_interr = false;
volatile boolean enc2_switch_interr = false;
volatile boolean enc3_switch_interr = false;
volatile boolean enc4_switch_interr = false;
volatile boolean enc_switch_global = false;


volatile char e_num;
volatile char e_dir;

void setup_switch_interrMCP(int pin){
  // configuration for a button on port A
  // interrupt will triger when the pin is taken to ground by a pushbutton
  mcp.pinMode(pin, INPUT);
  mcp.pullUp(pin, HIGH);  // turn on a 100K pullup internally
  mcp.setupInterruptPin(pin,CHANGE);   
}
void button_action(){
  button_to_read = true;
  digitalWrite(ledPin, HIGH);
}
void setup_button_interr(int pin){
  pinMode(pin, INPUT_PULLUP);
   attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(pin), button_action, FALLING);

}
void setup_encoder(int A, int B, int S, void (*bISR)(), void *(*switchISR)()){
  pinMode(S, INPUT_PULLUP);
  pinMode(A, INPUT_PULLUP);
  pinMode(B, INPUT_PULLUP);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(B), bISR, CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(S), switchISR, FALLING);
}
void enc1_switch_handler(){
  enc1_switch_interr = true;
  enc_switch_global = true;
}
void enc2_switch_handler(){
  enc2_switch_interr = true;
  enc_switch_global = true;
}
void enc3_switch_handler(){
  enc3_switch_interr = true;
  enc_switch_global = true;
}
void enc4_switch_handler(){
  enc4_switch_interr = true;
  enc_switch_global = true;
}
void setup(){

  Serial.begin(9600);
  Serial.println("pan");

  



  setup_encoder(E1_A, E1_B, E1_S, e1_update, enc1_switch_handler);
  setup_encoder(E2_A, E2_B, E2_S, e2_update, enc2_switch_handler); 
  setup_encoder(E3_A, E3_B, E3_S, e3_update, enc3_switch_handler);
  setup_encoder(E4_A, E4_B, E4_S, e4_update, enc4_switch_handler);
   
  //MCP
  pinMode(arduinoIntPin,INPUT);
  mcp.begin();      // use default address 0
  //LEDS
  mcp.pinMode(D1, OUTPUT);
  mcp.pinMode(D2, OUTPUT);
  mcp.pinMode(D3, OUTPUT);
  mcp.pinMode(D4, OUTPUT);
  mcp.pinMode(D5, OUTPUT);
  mcp.pinMode(D6, OUTPUT);
  mcp.digitalWrite(D1, HIGH);
  mcp.digitalWrite(D2, HIGH);
  mcp.digitalWrite(D3, HIGH);
  mcp.digitalWrite(D4, HIGH);
  mcp.digitalWrite(D5, HIGH);
  mcp.digitalWrite(D6, HIGH);
//  // We mirror INTA and INTB, so that only one line is required between MCP and Arduino for int reporting
//  // The INTA/B will not be Floating 
//  // INTs will be signaled with a LOW
  mcp.setupInterrupts(true,false,LOW);
//
  setup_switch_interrMCP(S1);
  setup_switch_interrMCP(S2);
  setup_switch_interrMCP(S3);
  setup_switch_interrMCP(S4);
  setup_switch_interrMCP(S5);
  setup_switch_interrMCP(S6);
  setup_switch_interrMCP(S7);
  setup_switch_interrMCP(S8);
//
  setup_button_interr(B1);
  setup_button_interr(B2);
  setup_button_interr(B3);
  setup_button_interr(B4);

  // We will setup a pin for flashing from the int routine
  pinMode(ledPin, OUTPUT);  // use the p13 LED as debugging
  digitalWrite(ledPin, HIGH);
  delay(250);
  digitalWrite(ledPin, LOW);
}

// The int handler will just signal that the int has happen
// we will do the work from the main loop.
void intCallBack(){
  awakenByInterruptMCP=true;
  digitalWrite(ledPin, HIGH);
}

void handleInterrupt(){
  
  // Get more information from the MCP from the INT
  uint8_t pin=mcp.getLastInterruptPin();
  uint8_t val=mcp.getLastInterruptPinValue();

  if(pin - 8 > 7){
    Serial.print(pin);
    Serial.println("RRR");
  }else{
//    Serial.print(pin);
    Serial.println(switches[pin-8]);
    Serial.flush();
//Serial.print(pin);
  }
    digitalWrite(ledPin,HIGH);
    delay(30);                //TO TEST
    digitalWrite(ledPin,LOW);
    delay(30);
  // we have to wait for the interrupt condition to finish
  // otherwise we might go to sleep with an ongoing condition and never wake up again.
  // as, an action is required to clear the INT flag, and allow it to trigger again.
  // see datasheet for datails.
  //  while( ! (mcp.digitalRead(mcpPinB) && mcp.digitalRead(mcpPinA) ));
  // and clean queued INT signal
  cleanInterrupts();
}

// handy for interrupts triggered by buttons
// normally signal a few due to bouncing issues
void cleanInterrupts(){
//  EIFR=0x01;
  awakenByInterruptMCP=false;
}  

/**
 * main routine: sleep the arduino, and wake up on Interrups.
 * the LowPower library, or similar is required for sleeping, but sleep is simulated here.
 * It is actually posible to get the MCP to draw only 1uA while in standby as the datasheet claims,
 * however there is no stadndby mode. Its all down to seting up each pin in a way that current does not flow.
 * and you can wait for interrupts while waiting.
 */
void serial_handler(){
  serial_to_read = true;
}
void read_serial(){
  char msg[5];
  int cnt = 0;
  while(Serial.available() < 4){
    if(cnt == 5){
      while(Serial.available() > 0) Serial.read(); // TO TEST
      serial_to_read = false;
      return;
    }
    delay(10);
    cnt++;
  }
  msg[0] = Serial.read();
  msg[1] = Serial.read();
  msg[2] = Serial.read();
  msg[3] = Serial.read(); 
  msg[4] = '\0';
  Serial.read();
  
  if(msg[0] == 'd'){
    int num = msg[1] - 48;
    if(msg[2] == 'n'){
      led_on(num);
    }else if(msg[2] == 'f'){
      led_off(num);
    }
  }
  Serial.println(msg);
  if(Serial.available() > 0){
    read_serial();
  }
  serial_to_read = false;
}
void loop(){
//  Serial.println("loop-up");
   Serial.flush();
   attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(SERIAL_RX_PIN), serial_handler, CHANGE);
  // enable interrupts before going to sleep/wait
  // And we setup a callback for the arduino INT handler.
  attachInterrupt(digitalPinToInterrupt(2),intCallBack,FALLING);

  
  // Simulate a deep sleep
  while(!(awakenByInterruptMCP || serial_to_read || button_to_read || encoder_to_send || enc_switch_global));
  // Or sleep the arduino, this lib is great, if you have it.
  //LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
//  Serial.println("after while");

  
  // disable interrupts while handling them.
  detachInterrupt(digitalPinToInterrupt(2));
  detachPinChangeInterrupt(digitalPinToPinChangeInterrupt(SERIAL_RX_PIN));
  if(awakenByInterruptMCP) handleInterrupt();
  if(serial_to_read) read_serial();
  if(button_to_read) button_pressed();
  if(encoder_to_send) encoder_handler();
  if(enc_switch_global) enc_switch_action();
//  Serial.println("loop-down");
}

void button_pressed(){
  if(digitalRead(B1) == LOW){
    Serial.println("b1p");
  }else if(digitalRead(B2) == LOW){
    Serial.println("b2p");
  }else if(digitalRead(B3) == LOW){
    Serial.println("b3p");
  }else if(digitalRead(B4) == LOW){
    Serial.println("b4p");
  }
  delay(30);
  while(!(digitalRead(B1) && digitalRead(B2) && digitalRead(B3) && digitalRead(B4)));
  button_to_read = false;
}

void encoder_handler(){
  Serial.print('e');
  Serial.print(e_num);
  Serial.print(e_dir);
  Serial.println();
  encoder_to_send = false;
}
void enc_switch_action(){
  if(enc1_switch_interr){
    Serial.println("e1s");
  }
  if(enc2_switch_interr){
    Serial.println("e2s");
    enc2_switch_interr = false;
  }
  if(enc3_switch_interr){
    Serial.print("e3s");
    Serial.print(digitalRead(E3_S));
    Serial.println();
  }
  if(enc4_switch_interr){
    Serial.println("e4s");
    enc4_switch_interr = false;
  }
  while(!(digitalRead(E1_S) && digitalRead(E2_S) && digitalRead(E3_S) && digitalRead(E4_S))){
//    Serial.print(digitalRead(E1_S));
//    Serial.print(digitalRead(E2_S));
//    Serial.print(digitalRead(E3_S));
//    Serial.print(digitalRead(E4_S));
//    Serial.println();
//    delay(1);
  }
  delay(50);
  enc1_switch_interr = false;
  enc2_switch_interr = false;
  enc3_switch_interr = false;
  enc_switch_global = false;
  enc4_switch_interr = false;
}

void e1_update(){
  
  /* WARNING: For this example I've used Serial.println within the interrupt callback. The Serial 
   * library already uses interrupts which could cause errors. Therefore do not use functions 
   * of the Serial libray in your interrupt callback.
   */
   digitalWrite(ledPin, HIGH);

  // ROTATION DIRECTION
  E1pinAstateCurrent = analogRead(E1_A) > 100 ? HIGH : LOW;    // on Pro Mini you cant use digitalRead on pins A6 and A7. Dont know why.

  // If there is a minimal movement of 1 step
  if ((E1pinAStateLast == LOW) && (E1pinAstateCurrent == HIGH)) {
    e_num = '1';
    encoder_to_send = true;
    if (digitalRead(E1_B) == HIGH) {      // If Pin B is HIGH
       e_dir = 'r';          // Print on screen
    } else {
      e_dir = 'l';            // Print on screen
    }
    
  }
  
  E1pinAStateLast = E1pinAstateCurrent;        // Store the latest read value in the currect state variable
  
}
void e2_update(){
  E2pinAstateCurrent = analogRead(E2_A) > 100 ? HIGH : LOW;    // on Pro Mini you cant use digitalRead on pins A6 and A7. Dont know why.
  
  // If there is a minimal movement of 1 step
  if ((E2pinAStateLast == LOW) && (E2pinAstateCurrent == HIGH)) {
    e_num = '2';    
    encoder_to_send = true;
    if (digitalRead(E2_B) == HIGH) {      // If Pin B is HIGH
       e_dir = 'r';          // Print on screen
    } else {
      e_dir = 'l';            // Print on screen
    }
    
  }
  E2pinAStateLast = E2pinAstateCurrent;        // Store the latest read value in the currect state variable
  
}
void e3_update(){
  E3pinAstateCurrent = digitalRead(E3_A);    // Read the current state of Pin A
  
  // If there is a minimal movement of 1 step
  if ((E3pinAStateLast == LOW) && (E3pinAstateCurrent == HIGH)) {
      encoder_to_send = true;
      e_num = '3';
    if (digitalRead(E3_B) == HIGH) {      // If Pin B is HIGH
       e_dir = 'r';          // Print on screen
    } else {
      e_dir = 'l';            // Print on screen
    }
    
  }
  E3pinAStateLast = E3pinAstateCurrent;        // Store the latest read value in the currect state variable
  
}
void e4_update(){
  E4pinAstateCurrent = digitalRead(E4_A);    // Read the current state of Pin A
  // If there is a minimal movement of 1 step
  if ((E4pinAStateLast == LOW) && (E4pinAstateCurrent == HIGH)) {
    e_num = '4';
    encoder_to_send = true;
    if (digitalRead(E4_B) == HIGH) {      // If Pin B is HIGH
       e_dir = 'r';          // Print on screen
    } else {
      e_dir = 'l';            // Print on screen
    }
    
  }
  E4pinAStateLast = E4pinAstateCurrent;        // Store the latest read value in the currect state variable
  
}
