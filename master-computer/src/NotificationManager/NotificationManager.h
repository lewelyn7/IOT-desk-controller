#ifndef __NOTIFICATIONMANAGER_H__
#define __NOTIFICATIONMANAGER_H__
#include <cppQueue.h>
#include <Arduino.h>
#define NOTFICATION_MSG_SIZE 32
enum class NotiPriority{ DEBUG, INFO, WARNING, ERROR};
enum class NotiType { Default_type, LED_ONLY, LCD_ONLY};
class Notification{
    public:

        NotiPriority pri;
        NotiType type;
        uint8_t time;
        char mess[NOTFICATION_MSG_SIZE];
        Notification(NotiPriority pri, uint8_t time, char mess[], NotiType type = NotiType::Default_type);
};

class NotificationManager{
    public:
        Queue notifs;
        NotificationManager();
        Notification * curr = NULL;
        void push_new(NotiPriority pri, uint8_t time, char str[], NotiType type = NotiType::Default_type);
        void tick();

    private:

};
#endif // __NOTIFICATIONMANAGER_H__