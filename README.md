## EasyBlynk8266
If you are hardcoding your Wifi and Blynk credentials, and reflashing everytime when you need to change them, please stop it, seriously. I was like that before, but one day I woke up, and said enough is enough.
I understand than if you do Arduino as a hobby, it's quite difficult to make time to write unnecessary features. But, with this library, you can do it without writing any extra code. Just replace, `Blynk` with `EasyBlynk8266` in your code, then it should just work fine, technically.

## So, how it works?
If it cannot connect to the Blynk server in 30 seconds, it just switches to `Configuration Mode`. You will see your built-in LED (if you have one) flashes every second. In `Configuration Mode`, it starts an access point called `EasyBlynk8266-Config`, like below. Connect to it.

<p align="center">
    <img src="https://user-images.githubusercontent.com/1836721/49258233-fcb50a80-f434-11e8-8e91-14a60202e487.png">
</p>

After you connected, please, go to http://192.168.1.1. You will see a page showed as below.
<p align="center">
    <img src="https://user-images.githubusercontent.com/1836721/49258236-ff176480-f434-11e8-9430-61cb09d8e6bd.png">
</p>

Enter your credentials, then click `Save`. After you restarted, you will see your built-in LED flashes 3 times. That means, it connected to your Blynk server successfully.

## Prerequisite
* ESP8266 core for Arduino https://github.com/esp8266/Arduino#installing-with-boards-manager
* Blynk library https://www.arduino.cc/en/guide/libraries#toc3

## Hello World
Please take a look at examples, as well.
```
#include <EasyBlynk8266.h>

void setup() {
    EasyBlynk8266.begin();
}


void loop() {
    EasyBlynk8266.run();
}
```
