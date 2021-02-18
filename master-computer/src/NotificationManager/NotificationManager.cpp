
#include "NotificationManager.h"
Notification::Notification(NotiPriority pri, uint8_t time, char mess[], NotiType type) 
{
    this->pri = pri;
    this->time = time;
    this->type = type;
    strncpy(this->mess, mess, NOTFICATION_MSG_SIZE);
}
NotificationManager::NotificationManager(): notifs(sizeof(Notification*), 20, FIFO)
{
    
}

void NotificationManager::push_new(NotiPriority pri, uint8_t time, char str[], NotiType type)
{
    Notification * noti = new Notification(pri, time, str);
    notifs.push(noti);
}

void NotificationManager::tick() 
{
    if(!curr){
        //null
    }else if(!notifs.isEmpty()){ //handle
        notifs.pop(curr);
        curr->time--;
        
        if(curr->type == NotiType::Default_type){

        }

        if(curr->time == 0){
            delete curr;
            curr = NULL;
        }
    }
}


