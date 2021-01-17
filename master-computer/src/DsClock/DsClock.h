#ifndef __TIME_H__
#define __TIME_H__

// #if defined(ESP8266)
// #include <pgmspace.h>
// #else
// #include <avr/pgmspace.h>
// #endif

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <Wire.h>
#include <RtcDS3231.h>


class DsClock{
    private:
        WiFiUDP ntpUDP;
        NTPClient timeClient;
        RtcDS3231<TwoWire> rtc;
        RtcDateTime time;
    public:
        DsClock();
        void update_time();
        void rtc_update();
        uint8_t getMinutes();
        uint8_t getHours();
        uint8_t getSeconds();
        
};
#endif // __TIME_H__