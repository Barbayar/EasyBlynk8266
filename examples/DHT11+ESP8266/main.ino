#include <EasyBlynk8266.h>
#include <DHT.h>

#define DHT_PIN 2
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);
BlynkTimer timer;

void readAndSendData() {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
        EasyBlynk8266.flash(2);

        return;
    }

    // NOTE: don't know why, but DHT11 was returning around 5°C higher value
    // maybe, because it was placed too close to ESP8266, and ESP8266 produces heat
    EasyBlynk8266.virtualWrite(V0, temperature - 5);
    EasyBlynk8266.virtualWrite(V1, humidity);
}

void setup() {
    dht.begin();
    EasyBlynk8266.begin();
    timer.setInterval(60 * 1000, readAndSendData);
}

void loop() {
    EasyBlynk8266.run();
    timer.run();
}
