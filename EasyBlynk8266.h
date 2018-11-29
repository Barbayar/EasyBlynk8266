#ifndef EasyBlynk8266_h
#define EasyBlynk8266_h

#include <ESP8266WiFi.h>
#include <Blynk.h>
#include <Blynk/BlynkProtocol.h>
#include <Adapters/BlynkArduinoClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

struct Configuration {
    char header[8];
    char wifi_ssid[64];
    char wifi_passphrase[64];
    char blynk_server[64];
    int blynk_port;
    char blynk_token[64];
};

class EasyBlynk8266Class: public BlynkProtocol<BlynkArduinoClient> {
    typedef BlynkProtocol<BlynkArduinoClient> Base;
public:
    EasyBlynk8266Class(BlynkArduinoClient& transp);
    void begin();
    void run();
    void flash(int n);
private:
    ESP8266WebServer server;
    BlynkTimer configuration_mode_led_timer;
    boolean configuration_mode = false;
    struct Configuration config;

    boolean connectToWifi(int timeout);
    void startConfigurationMode();
    void handleRequest();
};

extern EasyBlynk8266Class EasyBlynk8266;

#endif
