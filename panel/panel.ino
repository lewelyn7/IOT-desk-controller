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

#define D1 5
#define D2 4
#define D3 3
#define D4 2
#define D5 1
#define D6 0

void led_on(int num){
  if(num <= 6){
    digitalWrite(num, HIGH);
  }
  
}
void led_off(int num){
  if(num <= 6){
    digitalWrite(num, HIGH);
  }
  
}
#define S1 11
#define S2 10
#define S3 13
#define S4 12
#define S5 15
#define S6 14
#define S7 9
#define S8 8

#define E1_S 5
#define E1_A 10
#define E1_B 9

#define E2_S 11
#define E2_A 8
#define E2_B 4

#define E3_S 10
#define E3_A 9
#define E3_B 5

#define E4_S 13
#define E4_A 17
#define E4_B 6


#define B1 19
#define B2 22
#define B3 23
#define B4 24


int E1pinAstateCurrent = LOW;                // Current state of Pin A
int E1pinAStateLast = E1pinAstateCurrent;      // Last read value of Pin A

int E2pinAstateCurrent = LOW;                // Current state of Pin A
int E2pinAStateLast = E2pinAstateCurrent;      // Last read value of Pin A

int E3pinAstateCurrent = LOW;                // Current state of Pin A
int E3pinAStateLast = E3pinAstateCurrent;      // Last read value of Pin A

int E4pinAstateCurrent = LOW;                // Current state of Pin A
int E4pinAStateLast = E4pinAstateCurrent;      // Last read value of Pin A

char * switches[] = { "s8", "s7", "s2\0", "s1", "s4", "s3", "s6", "s5" };


//MCP
Adafruit_MCP23017 mcp;
byte ledPin=13;
#define SERIAL_RX_PIN 0
// Interrupts from the MCP will be handled by this PIN
byte arduinoIntPin=2;
volatile boolean awakenByInterruptMCP = false;

volatile boolean serial_to_read = false;

void setup_switch_interrMCP(int pin){
  // configuration for a button on port A
  // interrupt will triger when the pin is taken to ground by a pushbutton
  mcp.pinMode(pin, INPUT);
  mcp.pullUp(pin, HIGH);  // turn on a 100K pullup internally
  mcp.setupInterruptPin(pin,CHANGE);   
}

void setup_button_interr(int pin){
  pinMode(pin, INPUT_PULLUP);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(pin), button_pressed, CHANGE);

}
void setup(){

  Serial.begin(9600);
  Serial.println("MCP23007 Interrupt Test");



  //encoders
  pinMode(E1_S, INPUT_PULLUP);
  pinMode(E1_A, INPUT);
  pinMode(E1_B, INPUT);
  
  pinMode(E2_S, INPUT_PULLUP);
  pinMode(E2_A, INPUT);
  pinMode(E2_B, INPUT);

  pinMode(E3_S, INPUT_PULLUP);
  pinMode(E3_A, INPUT);
  pinMode(E3_B, INPUT);

  pinMode(E4_S, INPUT_PULLUP);
  pinMode(E4_A, INPUT);
  pinMode(E4_B, INPUT);

  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(E1_B), e1_update, CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(E2_B), e2_update, CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(E3_B), e3_update, CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(E4_B), e4_update, CHANGE);
   
  //MCP
  pinMode(arduinoIntPin,INPUT_PULLUP);
  mcp.begin();      // use default address 0
  // We mirror INTA and INTB, so that only one line is required between MCP and Arduino for int reporting
  // The INTA/B will not be Floating 
  // INTs will be signaled with a LOW
  mcp.setupInterrupts(true,false,LOW);

  setup_switch_interrMCP(S1);
  setup_switch_interrMCP(S2);
  setup_switch_interrMCP(S3);
  setup_switch_interrMCP(S4);
  setup_switch_interrMCP(S5);
  setup_switch_interrMCP(S6);
  setup_switch_interrMCP(S7);
  setup_switch_interrMCP(S8);

  setup_button_interr(B1);
  setup_button_interr(B2);
  setup_button_interr(B3);
  setup_button_interr(B4);

  // We will setup a pin for flashing from the int routine
  pinMode(ledPin, OUTPUT);  // use the p13 LED as debugging
  
}

// The int handler will just signal that the int has happen
// we will do the work from the main loop.
void intCallBack(){
  awakenByInterruptMCP=true;
}

void handleInterrupt(){
  
  // Get more information from the MCP from the INT
  uint8_t pin=mcp.getLastInterruptPin();
  uint8_t val=mcp.getLastInterruptPinValue();
  if(pin - 8 > 7){
    Serial.print("RRR");
  }else{
    Serial.print(switches[pin]);
  }
    delay(50);
    digitalWrite(ledPin,HIGH);
    delay(50);
    digitalWrite(ledPin,LOW);
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
  byte msg[4];
  int cnt = 0;
  while(Serial.available() < 3){
    if(cnt == 5){
      return;
    }
    delay(10);
    cnt++;
  }
  msg[0] = Serial.read();
  msg[1] = Serial.read();
  msg[2] = Serial.read();
  msg[3] = "\0";

  if(msg[0] = 'd'){
    int num = msg[1] - 48;
    if(msg[2] = 'n'){
      led_on(num);
    }else if(msg[2] = 'f'){
      led_off(num);
    }
  }
  
  if(Serial.available() > 0){
    read_serial();
  }
  serial_to_read = false;
}
void loop(){
   Serial.flush();
   attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(SERIAL_RX_PIN), serial_handler, CHANGE);
  // enable interrupts before going to sleep/wait
  // And we setup a callback for the arduino INT handler.
  attachInterrupt(0,intCallBack,FALLING);

  
  // Simulate a deep sleep
  while(!awakenByInterruptMCP);
  // Or sleep the arduino, this lib is great, if you have it.
  //LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);

  
  // disable interrupts while handling them.
  detachInterrupt(0);
  detachPinChangeInterrupt(digitalPinToPinChangeInterrupt(SERIAL_RX_PIN));
  if(awakenByInterruptMCP) handleInterrupt();
  if(serial_to_read) read_serial();
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
}

void e1_update(){
  
  /* WARNING: For this example I've used Serial.println within the interrupt callback. The Serial 
   * library already uses interrupts which could cause errors. Therefore do not use functions 
   * of the Serial libray in your interrupt callback.
   */

  // ROTATION DIRECTION
  E1pinAstateCurrent = digitalRead(E1_A);    // Read the current state of Pin A
  
  // If there is a minimal movement of 1 step
  if ((E1pinAStateLast == LOW) && (E1pinAstateCurrent == HIGH)) {
    
    if (digitalRead(E1_B) == HIGH) {      // If Pin B is HIGH
      Serial.println("e1r");             // Print on screen
    } else {
      Serial.println("e1l");            // Print on screen
    }
    
  }
  
  E1pinAStateLast = E1pinAstateCurrent;        // Store the latest read value in the currect state variable
  
}
void e2_update(){
  E2pinAstateCurrent = digitalRead(E2_A);    // Read the current state of Pin A
  
  // If there is a minimal movement of 1 step
  if ((E2pinAStateLast == LOW) && (E2pinAstateCurrent == HIGH)) {
    
    if (digitalRead(E2_B) == HIGH) {      // If Pin B is HIGH
      Serial.println("e2r");             // Print on screen
    } else {
      Serial.println("e2l");            // Print on screen
    }
    
  }
  E2pinAStateLast = E2pinAstateCurrent;        // Store the latest read value in the currect state variable
  
}
void e3_update(){
  E3pinAstateCurrent = digitalRead(E3_A);    // Read the current state of Pin A
  
  // If there is a minimal movement of 1 step
  if ((E3pinAStateLast == LOW) && (E3pinAstateCurrent == HIGH)) {
    
    if (digitalRead(E3_B) == HIGH) {      // If Pin B is HIGH
      Serial.println("e3r");             // Print on screen
    } else {
      Serial.println("e3l");            // Print on screen
    }
    
  }
  E3pinAStateLast = E3pinAstateCurrent;        // Store the latest read value in the currect state variable
  
}
void e4_update(){
  E4pinAstateCurrent = digitalRead(E4_A);    // Read the current state of Pin A
  // If there is a minimal movement of 1 step
  if ((E4pinAStateLast == LOW) && (E4pinAstateCurrent == HIGH)) {
    
    if (digitalRead(E4_B) == HIGH) {      // If Pin B is HIGH
      Serial.println("e4r");             // Print on screen
    } else {
      Serial.println("e4l");            // Print on screen
    }
    
  }
  E4pinAStateLast = E4pinAstateCurrent;        // Store the latest read value in the currect state variable
  
}
