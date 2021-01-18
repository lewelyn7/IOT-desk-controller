#include "DsClock.h"
DsClock::DsClock():timeClient(ntpUDP),rtc(Wire)
{
    timeClient.begin();
    rtc.Begin();
}

void DsClock::update_time() 
{
    timeClient.update();
    //TODO date update
    RtcDateTime dt = RtcDateTime(2021,1,17,
        (timeClient.getHours() + 1)%24, 
        timeClient.getMinutes(), 
        timeClient.getSeconds());
    //ERROR HANDLING INSTRUCTION IN LIB ON GITHUB
    rtc.SetDateTime(dt);
}

void DsClock::rtc_update() 
{
    this->time = rtc.GetDateTime();
}

uint8_t DsClock::getMinutes() 
{
    return this->time.Minute();
}
uint8_t DsClock::getHours() 
{
    return this->time.Hour();
}

uint8_t DsClock::getSeconds() 
{
    return this->time.Second();
}
