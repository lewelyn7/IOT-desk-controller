
#include "MQTTCommunicator.h"

const char * desk_last_will_topic = "desk/availability";
const char * desk_last_will_payload = "offline";

const char * led_strip_state = "desk/strip1/light/status";
const char * led_strip_available = "desk/strip1/light/ava";
const char * led_strip_cmd = "desk/strip1/light/switch";
const char * led_strip_brig_state = "desk/strip1/light/brightness/status";
const char * led_strip_brig_cmd = "desk/strip1/light/brightness/set";
const char * led_strip_hs_state = "desk/strip1/light/hs/status";
const char * led_strip_hs_cmd = "desk/strip1/light/hs/set";
const char * led_strip_effect_cmd = "desk/strip1/light/set";
const char * led_strip_effect_state = "desk/strip1/light/status";
const char * led_strip_effect_list = "desk/strip1/light/list";

const char * bulb_state = "desk/bulb/light/status";
const char * bulb_available = "desk/bulb/light/ava";
const char * bulb_cmd = "desk/bulb/light/switch";

const char * led_screen_general = "desk/ledscreen/general";
const char * led_screen_general_t_set = "desk/ledscreen/general/time";
const char * led_screen_state = "desk/ledscreen/status";
const char * led_screen_available = "desk/ledscreen/ava";
const char * led_screen_cmd = "desk/ledscreen/switch";
const char * desk_led_scr_mode_cmd = "desk/ledscreen/mode/cmd";
const char * desk_led_scr_mode_status = "desk/ledscreen/mode/status";
void print_byte_string(char * str, unsigned int len) 
{
    for(int i = 0; i < len; i++){
        Serial.print(str[i]);
    }
    Serial.print("\r\n");
}
void message_callback(char* topic, byte* message, unsigned int length) 
{
#ifdef DEBUG
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    print_byte_string((char*) message, length);
#endif
    char reply_msg[32];
    if(!strcmp(topic, led_strip_cmd)){
        if(!strncmp((char*) message, "ON", length) && !animationManager->master_layer->master_switch){
            animationManager->master_layer->On();
            panel->led_on(0);
            mqtt->light_strip1_on();
        }else if(!strncmp((char*) message, "OFF", length) && animationManager->master_layer->master_switch){
            panel->led_off(0);
            animationManager->master_layer->Off();
            mqtt->light_strip1_off();
        }
    }if(!strcmp(topic, led_strip_brig_cmd)){
        strncpy(reply_msg, (char*)message, length);
        reply_msg[length] = '\0';
        int value = (uint8_t)atoi(reply_msg);
        animationManager->get_current()->setV(value);
        mqtt->update_led_brightness();
    }else if(!strcmp(topic, led_strip_hs_cmd)){
        strncpy(reply_msg, (char*)message, length);
        reply_msg[length] = '\0';
        float h;
        float s;
        sscanf(reply_msg, "%f,%f", &h, &s);
        animationManager->get_current()->setH(((int)(h*255.0/360.0))%256);
        animationManager->get_current()->setS(((int)(s*255.0/100.0))%256);
        mqtt->update_led_strip_hsv();
    }else if(!strcmp(topic, bulb_cmd)){
         if(!strncmp((char*) message, "ON", length)){
            panel->led_on(2);
            digitalWrite(LAPTOP_FAN, HIGH);
            mqtt->desk_bulb_on();
        }else if(!strncmp((char*) message, "OFF", length)){
            panel->led_off(2);
            digitalWrite(LAPTOP_FAN, LOW);
            mqtt->desk_bulb_off();
        }       
    }else if(!strcmp(topic, led_screen_cmd)){
        if(!strncmp((char*) message, "ON", length)){
            screen->on();
            panel->led_on(1);
            mqtt->led_screen_on();
        }
        else if(!strncmp((char*) message, "OFF", length)){
            screen->off();
            panel->led_off(1);
            mqtt->led_screen_off();
        }
    }else if(!strcmp(topic, desk_led_scr_mode_cmd)){
        //strncpy(reply_msg, (char*)message, length);
        if(!strncmp((char*) message, "time", length)){
            screen->setMode(LedScreenModes::Time);
        }else if(!strncmp((char*) message, "timer", length)){
            screen->setMode(LedScreenModes::Timer);
        }else if(!strncmp((char*) message, "general", length)){
            screen->setMode(LedScreenModes::General);
        }else if(!strncmp((char*) message, "temp", length)){
            screen->setMode(LedScreenModes::Temp);
        }    
        mqtt->update_led_scr_mode();
        
    }else if(!strcmp(topic, led_screen_general)){
        if(length > 4){
            if(message[0] == '@'){
                screen->copyToGeneralBuff((char*)message+1, length-1);
                screen->general_buffer_replay = true;
            }else{
                screen->copyToGeneralBuff((char*)message+1, length-1);
                screen->general_buffer_replay = false;
            }

        }else if(length == 4){
            screen->display((int8_t*) message, &screen->general );
        }

    }else if(!strcmp(topic, led_screen_general_t_set)){
        strncpy(reply_msg, (char*)message, length);
        reply_msg[length] = '\0';
        uint8_t value = (uint8_t)atoi(reply_msg);        
        screen->time_iter_setting = value;
    }
}
void MQTTCommunicator::update_led_brightness() 
{
    int value = animationManager->get_current()->v;
    sprintf(reply_msg, "{\"brightness\": %d}", value);
    mqtt->send_msg(led_strip_brig_state, reply_msg);
}

void MQTTCommunicator::led_screen_on() 
{
    mqtt->send_msg(led_screen_state, "ON");
}

void MQTTCommunicator::led_screen_off() 
{
    mqtt->send_msg(led_screen_state, "OFF");
}

void MQTTCommunicator::update_led_strip_hsv() 
{
    Animation * anim = animationManager->get_current();
    sprintf(reply_msg, "{\"hs\": [%d,%d]}", anim->h*360/255, anim->s*100/255);
    mqtt->send_msg(led_strip_hs_state, reply_msg);
}
MQTTCommunicator::MQTTCommunicator() 
: client(espClient)
{
    Serial.println("setup wifi..");
    Serial.print("ssid:");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    Serial.print("connecting");
    while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
    }
    Serial.println("\nconnected");
    Serial.println(WiFi.localIP());
    client.setServer(mqtt_server, 1883);
    client.setCallback(message_callback);
    client.setKeepAlive(30);
    client.setSocketTimeout(45);
    mqtt_reconnect();
    Serial.println("topic subscribe");

    Serial.println("topic publish");
    send_msg(led_strip_available, "online");
    Serial.println("done");

    
}




void MQTTCommunicator::mqtt_reconnect() 
{
  // Loop until we're reconnected

  if(!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-master";

    // Attempt to connect
    if (client.connect(clientId.c_str(), desk_last_will_topic, 1, 1, desk_last_will_payload)) {
      Serial.println("MQTT connected");
      // Once connected, publish an announcement...
        if(!client.subscribe(led_strip_cmd)){
            Serial.println("subscription error");
        }
        if(!client.subscribe(bulb_cmd)){
            Serial.println("subscription error");
        }
        if(!client.subscribe(led_strip_hs_cmd)){
            Serial.println("subscription error");
        }
        if(!client.subscribe(led_strip_brig_cmd)){
            Serial.println("subscription error");
        }         
        if(!client.subscribe(led_screen_cmd)){
            Serial.println("subscription error");
        }   
        if(!client.subscribe(desk_led_scr_mode_cmd)){
            Serial.println("subscription error");
        }      
        if(!client.subscribe(led_screen_general_t_set)){
            Serial.println("subscription error");
        } 
        if(!client.subscribe(led_screen_general)){
            Serial.println("subscription error");
        }                                
        client.publish(led_strip_available, "online");
        client.publish(bulb_available, "online");
        client.publish(led_screen_available, "online");
        client.publish(desk_last_will_topic, "online");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 1 seconds");
      // Wait 5 seconds before retrying
    }
  }
}

void MQTTCommunicator::send_msg(const char * topic, const char * message) 
{
    if (!client.connected()) {
        mqtt_reconnect();
    }
    
    if(!client.publish(topic, message)){
        Serial.println("publish error");
    }
  }

void MQTTCommunicator::loop() 
{
    client.loop();
}

void MQTTCommunicator::desk_bulb_off() 
{
    send_msg(bulb_state, "OFF");
}

void MQTTCommunicator::desk_bulb_on() 
{
    send_msg(bulb_state, "ON");
}

void MQTTCommunicator::light_strip1_off() 
{
    mqtt->send_msg(led_strip_state, "{\"state\": \"OFF\"}");
}

void MQTTCommunicator::light_strip1_on() 
{
    mqtt->send_msg(led_strip_state, "{\"state\": \"ON\"}");
}
void MQTTCommunicator::update_led_scr_mode()
{
    mqtt->send_msg(desk_led_scr_mode_status, screen->get_mode_str_representation());
}





// 1612000338: Sending PINGRESP to mqttdash-066c61e6
// 1612000346: Received PUBLISH from ESP8266Client-c7e2 (d0, q0, r0, m0, 'desk/strip1/light/hs/status', ... (18 bytes))
// 1612000346: Sending PUBLISH to 2wQyd6ZozcE5A2uQpbxfZL (d0, q0, r0, m0, 'desk/strip1/light/hs/status', ... (18 bytes))
// 1612000346: Sending PUBLISH to mqttdash-066c61e6 (d0, q0, r0, m0, 'desk/strip1/light/hs/status', ... (18 bytes))
// 1612000346: Received PUBLISH from ESP8266Client-c7e2 (d0, q0, r0, m0, 'desk/strip1/light/hs/status', ... (18 bytes))
// 1612000346: Sending PUBLISH to 2wQyd6ZozcE5A2uQpbxfZL (d0, q0, r0, m0, 'desk/strip1/light/hs/status', ... (18 bytes))
// 1612000346: Sending PUBLISH to mqttdash-066c61e6 (d0, q0, r0, m0, 'desk/strip1/light/hs/status', ... (18 bytes))
// 1612000365: Received PINGREQ from ESP8266Client-c7e2
// 1612000365: Sending PINGRESP to ESP8266Client-c7e2
// 1612000365: Client ESP8266Client-44de has exceeded timeout, disconnecting.
// 1612000365: Sending PUBLISH to 2wQyd6ZozcE5A2uQpbxfZL (d0, q0, r0, m0, 'desk/availability', ... (7 bytes))
// 1612000365: Sending PUBLISH to mqttdash-066c61e6 (d0, q0, r0, m0, 'desk/availability', ... (7 bytes))
