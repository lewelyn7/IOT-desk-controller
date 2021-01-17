
#include "MQTTCommunicator.h"
const char * led_strip_state = "desk/strip1/light/status";
const char * led_strip_available = "desk/strip1/light/ava";
const char * led_strip_cmd = "desk/strip1/light/switch";
const char * led_strip_brig_state = "desk/strip1/light/brightness/status";
const char * led_strip_brig_cmd = "desk/strip1/light/brightness/set";
const char * led_strip_rgb_state = "desk/strip1/light/rgb/status";
const char * led_strip_rgb_cmd = "desk/strip1/light/rgb/set";
const char * led_strip_effect_cmd = "desk/strip1/light/set";
const char * led_strip_effect_state = "desk/strip1/light/status";
const char * led_strip_effect_list = "desk/strip1/light/list";

const char * bulb_state = "desk/bulb/light/status";
const char * bulb_available = "desk/bulb/light/ava";
const char * bulb_cmd = "desk/bulb/light/switch";

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
    }else if(!strcmp(topic, led_strip_rgb_cmd)){
        strncpy(reply_msg, (char*)message, length);
        reply_msg[length] = '\0';
        int r;
        int g;
        int b;
        // char * firstIndex = strchr(reply_msg, ',');
        // char * secondIndex = strchr(firstIndex+1, ',');
        // zawartość aktualizacji RIPA z horyzontem z wyłączonem horyzontem
        // jak działają tracreouty, rejestracje TRAS 
        // że zmieści się nie więcej niż 9 adresów
        // od ARPA zaczynamy
        // kończąc na 3 warstwie razem z translacją adresów
        // TCP UDP - część ustan przy kolokwium praktyczntm
        // ustno-praktyczne w ostatnim tygodniu krótkimn
        // między 40 a 60 min trudniejsza część ustna 
        // jak policzyć te punkty do anihilacji
        // zademonstrować funkcjonowanie dzielonego horyzontu w najprostszej możliwej topologii
        sscanf(reply_msg, "%d,%d,%d", &r, &g, &b);
        CRGB color;         //one object creation
        color = CRGB(r,g,b);
        CHSV colorHSV = rgb2hsv_approximate(color); //TO CHECK
        animationManager->get_current()->setHSV(colorHSV.h, colorHSV.s, colorHSV.v);
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
    }
}
void MQTTCommunicator::update_led_brightness() 
{
    int value = animationManager->get_current()->v;
    sprintf(reply_msg, "{\"brightness\": %d}", value);
    mqtt->send_msg(led_strip_brig_state, reply_msg);
}

void MQTTCommunicator::update_led_strip_hsv() 
{
    Animation * anim = animationManager->get_current();
    CRGB color = CRGB();
    color.setHSV(anim->h, anim->s, anim->v);
    sprintf(reply_msg, "{\"rgb\": [%d,%d,%d]}", color.r, color.g, color.b);
    mqtt->send_msg(led_strip_rgb_state, reply_msg);
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
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT connected");
      // Once connected, publish an announcement...
        if(!client.subscribe(led_strip_cmd)){
            Serial.println("subscription error");
        }
        if(!client.subscribe(bulb_cmd)){
            Serial.println("subscription error");
        }
        if(!client.subscribe(led_strip_rgb_cmd)){
            Serial.println("subscription error");
        }
        if(!client.subscribe(led_strip_brig_cmd)){
            Serial.println("subscription error");
        }                
        client.publish(led_strip_available, "online");
        client.publish(bulb_available, "online");
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





