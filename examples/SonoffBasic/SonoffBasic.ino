#include <EasyBlynk8266.h>

#define PIN_BUTTON 0
#define PIN_RELAY 12

void setup() {
    pinMode(PIN_BUTTON, INPUT);
    pinMode(PIN_RELAY, OUTPUT);

    EasyBlynk8266.begin();
}

int lastButtonState = HIGH;
int lastRelayState = LOW;

void loop() {
    EasyBlynk8266.run();

    int currentButtonState = digitalRead(PIN_BUTTON);
    if (currentButtonState == LOW && currentButtonState != lastButtonState) {
        digitalWrite(PIN_RELAY, !digitalRead(PIN_RELAY));
    }
    lastButtonState = currentButtonState;

    int currentRelayState = digitalRead(PIN_RELAY);
    if (currentRelayState != lastRelayState) {
        EasyBlynk8266.flash(currentRelayState + 1);
    }
    lastRelayState = currentRelayState;
}
