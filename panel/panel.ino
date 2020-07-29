#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include "PinChangeInterrupt.h"

// Install the LowPower library for optional sleeping support.
// See loop() function comments for details on usage.
//#include <LowPower.h>

#include <Wire.h>
#include <Adafruit_MCP23017.h>
// Basic pin reading and pullup test for the MCP23017 I/O expander
// public domain!

// Connect pin #12 of the expander to Analog 5 (i2c clock)
// Connect pin #13 of the expander to Analog 4 (i2c data)
// Connect pins #15, 16 and 17 of the expander to ground (address selection)
// Connect pin #9 of the expander to 5V (power)
// Connect pin #10 of the expander to ground (common ground)
// Connect pin #18 through a ~10kohm resistor to 5V (reset pin, active low)

// Input #0 is on pin 21 so connect a button or switch from there to ground

#define D1 5
#define D2 4
#define D3 3
#define D4 2
#define D5 1
#define D6 0

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
int pinAstateCurrent = LOW;                // Current state of Pin A
int pinAStateLast = pinAstateCurrent;      // Last read value of Pin A

char * switches[] = { "s8\0", "s7\0", "s2\0", "s1", "s4", "s3", "s6", "s5" };
/*
#define B1
#define B2
#define B3
#define B4
*/



Adafruit_MCP23017 mcp;

byte ledPin=13;

// Interrupts from the MCP will be handled by this PIN
byte arduinoIntPin=2;


volatile boolean awakenByInterrupt = false;

// Two pins at the MCP (Ports A/B where some buttons have been setup.)
// Buttons connect the pin to grond, and pins are pulled up.

void setup_interrupt(int pin){
  // configuration for a button on port A
  // interrupt will triger when the pin is taken to ground by a pushbutton
  mcp.pinMode(pin, INPUT);
  mcp.pullUp(pin, HIGH);  // turn on a 100K pullup internally
  mcp.setupInterruptPin(pin,CHANGE);  
//  mcp.setupInterruptPin(pin,RISING); 
}

void setup(){

  Serial.begin(9600);
  Serial.println("MCP23007 Interrupt Test");

  pinMode(E1_S, INPUT_PULLUP);
  pinMode(E1_A, INPUT);
  pinMode(E1_B, INPUT);
  
  

  pinMode(arduinoIntPin,INPUT_PULLUP);

  mcp.begin();      // use default address 0
  
  // We mirror INTA and INTB, so that only one line is required between MCP and Arduino for int reporting
  // The INTA/B will not be Floating 
  // INTs will be signaled with a LOW
  mcp.setupInterrupts(true,false,LOW);

  setup_interrupt(S1);
  setup_interrupt(S2);
  setup_interrupt(S3);
  setup_interrupt(S4);
  setup_interrupt(S5);
  setup_interrupt(S6);
  setup_interrupt(8);
  setup_interrupt(0);


  // We will setup a pin for flashing from the int routine
  pinMode(ledPin, OUTPUT);  // use the p13 LED as debugging
  
}

// The int handler will just signal that the int has happen
// we will do the work from the main loop.
void intCallBack(){
  awakenByInterrupt=true;
}

void handleInterrupt(){
  
  // Get more information from the MCP from the INT
  uint8_t pin=mcp.getLastInterruptPin();
  uint8_t val=mcp.getLastInterruptPinValue();
  Serial.print(pin);
  // We will flash the led 1 or 2 times depending on the PIN that triggered the Interrupt
  // 3 and 4 flases are supposed to be impossible conditions... just for debugging.
  uint8_t flashes=4; 
//  if(pin==mcpPinA) flashes=1;
//  if(pin==mcpPinB) flashes=2;
  if(val!=LOW) flashes=3;

  // simulate some output associated to this
  for(int i=0;i<flashes;i++){  
    delay(100);
    digitalWrite(ledPin,HIGH);
    delay(100);
    digitalWrite(ledPin,LOW);
  }

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
  awakenByInterrupt=false;
}  

/**
 * main routine: sleep the arduino, and wake up on Interrups.
 * the LowPower library, or similar is required for sleeping, but sleep is simulated here.
 * It is actually posible to get the MCP to draw only 1uA while in standby as the datasheet claims,
 * however there is no stadndby mode. Its all down to seting up each pin in a way that current does not flow.
 * and you can wait for interrupts while waiting.
 */
void loop(){
   attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(E1_B), e1_update, CHANGE);
  // enable interrupts before going to sleep/wait
  // And we setup a callback for the arduino INT handler.
  attachInterrupt(0,intCallBack,FALLING);
  
  // Simulate a deep sleep
  while(!awakenByInterrupt);
  // Or sleep the arduino, this lib is great, if you have it.
  //LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
  
  // disable interrupts while handling them.
  detachInterrupt(0);
  
  if(awakenByInterrupt) handleInterrupt();
}

void e1_update(){
  Serial.println("movement");
  
  /* WARNING: For this example I've used Serial.println within the interrupt callback. The Serial 
   * library already uses interrupts which could cause errors. Therefore do not use functions 
   * of the Serial libray in your interrupt callback.
   */

  // ROTATION DIRECTION
  pinAstateCurrent = digitalRead(E1_A);    // Read the current state of Pin A
  
  // If there is a minimal movement of 1 step
  if ((pinAStateLast == LOW) && (pinAstateCurrent == HIGH)) {
    
    if (digitalRead(E1_B) == HIGH) {      // If Pin B is HIGH
      Serial.println("Right");             // Print on screen
    } else {
      Serial.println("Left");            // Print on screen
    }
    
  }
  
  pinAStateLast = pinAstateCurrent;        // Store the latest read value in the currect state variable
  
}
