#pragma once
#ifndef __GLOBALS_H__
#define __GLOBALS_H__



#define DATA_PIN_WS 5 // todo pins changed only for testing

// #define CLOCK_PIN 15
#define DEBUG 1
#define PIEZZO 10

#define LAPTOP_FAN 15
#define DHTPIN 13
#define DHTTYPE DHT11
#define SERIAL_RX_PIN 12
#define SERIAL_TX_PIN 14
#define FASTLED_ESP8266_RAW_PIN_ORDER

#include <FastLED.h>
#include "utils.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "LedDisplay/LedDisplay.h"
#include "Menu/Menu.h"
#include "Animation/animation.h"
#include "LedDisplay/LedDisplay.h"

extern Screen *screen;
extern SoftwareSerial pcSerial1; // rx, tx
extern CRGB leds[NUM_LEDS];
extern AnimationsManager *animationManager;
#endif // __GLOBALS_H__