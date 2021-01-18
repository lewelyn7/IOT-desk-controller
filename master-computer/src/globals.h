#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <SoftwareSerial.h>
#define DATA_PIN_WS 13 // todo pins changed only for testing

// #define DsClock_PIN 15

#define DEBUG 1
#define PIEZZO 10

#define LAPTOP_FAN 15
#define DHTPIN 10
#define DHTTYPE DHT11
#define SERIAL_RX_PIN 12
#define SERIAL_TX_PIN 14
#define FASTLED_ESP8266_RAW_PIN_ORDER
#define NUM_LEDS 90

class Panel;
class PanelHandler;
class AnimationsManager;
class Screen;
class MQTTCommunicator;
class DsClock;

extern DsClock *dsclock;
extern Screen *screen;
extern SoftwareSerial pcSerial1; // rx, tx
extern AnimationsManager *animationManager;
extern Panel *panel;
extern PanelHandler *phandler;
extern MQTTCommunicator *mqtt;

#endif // __GLOBALS_H__