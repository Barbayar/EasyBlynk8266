#include "EasyBlynk8266.h"

String root_html_template = " \
<!DOCTYPE html> \
<meta name=\"robots\" content=\"noindex\"> \
<html> \
<head> \
  <meta charset=\"utf-8\"> \
  <meta name=\"viewport\" content=\"width=device-width\"> \
  <title>EasyBlynk8266</title> \
</head> \
<body> \
  <link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/icon?family=Material+Icons\"> \
  <link rel=\"stylesheet\" href=\"https://code.getmdl.io/1.3.0/material.indigo-pink.min.css\"> \
  <script defer src=\"https://code.getmdl.io/1.3.0/material.min.js\"></script> \
  <div align=\"center\"> \
    <table class=\"mdl-data-table mdl-js-data-tablemdl-shadow--2dp\"> \
      <tbody> \
        <tr> \
          <th class=\"mdl-data-table__cell--non-numeric\">Wifi</th> \
        </tr> \
        <tr> \
          <td class=\"mdl-data-table__cell--non-numeric\"> \
            <div class=\"mdl-textfield mdl-js-textfield mdl-textfield--floating-label\"> \
              <input class=\"mdl-textfield__input\" type=\"text\" id=\"wifi_ssid\" value=\"[[wifi_ssid]]\"> \
              <label class=\"mdl-textfield__label\" for=\"wifi_ssid\">SSID</label> \
            </div> \
            <br /> \
            <div class=\"mdl-textfield mdl-js-textfield mdl-textfield--floating-label\"> \
              <input class=\"mdl-textfield__input\" type=\"text\" id=\"wifi_passphrase\" value=\"[[wifi_passphrase]]\"> \
              <label class=\"mdl-textfield__label\" for=\"wifi_passphrase\">Passphrase</label> \
            </div> \
          </td> \
        </tr> \
        <tr> \
          <th class=\"mdl-data-table__cell--non-numeric\">Blynk</th> \
        </tr> \
        <tr> \
          <td class=\"mdl-data-table__cell--non-numeric\"> \
            <div class=\"mdl-textfield mdl-js-textfield mdl-textfield--floating-label\"> \
              <input class=\"mdl-textfield__input\" type=\"text\" id=\"blynk_server\" value=\"[[blynk_server]]\"> \
              <label class=\"mdl-textfield__label\" for=\"blynk_server\">Server</label> \
            </div> \
            <br /> \
            <div class=\"mdl-textfield mdl-js-textfield mdl-textfield--floating-label\"> \
              <input class=\"mdl-textfield__input\" type=\"text\" id=\"blynk_port\" pattern=\"-?[0-9]*(\.[0-9]+)?\" value=\"[[blynk_port]]\"> \
              <label class=\"mdl-textfield__label\" for=\"blynk_port\">Port</label> \
            </div> \
            <br /> \
            <div class=\"mdl-textfield mdl-js-textfield mdl-textfield--floating-label\"> \
              <input class=\"mdl-textfield__input\" type=\"text\" id=\"blynk_token\" value=\"[[blynk_token]]\"> \
              <label class=\"mdl-textfield__label\" for=\"blynk_token\">Token</label> \
            </div> \
          </td> \
        </tr> \
        <tr> \
          <td> \
            <button class=\"mdl-button mdl-js-button mdl-button--raised mdl-js-ripple-effect\" onclick=\"save()\"> \
              Save \
            </button> \
          </td> \
        </tr> \
      </tbody> \
    </table> \
  </div> \
<script id=\"jsbin-javascript\"> \
function updateValue(key, value) { \
  var request = new XMLHttpRequest(); \
  var url = '/?key=' + key + '&value=' + value; \
  console.log('calling ' + url + '...'); \
  request.open('GET', url, false); \
  request.send(null); \
} \
function save() { \
  updateValue('wifi_ssid', document.getElementById('wifi_ssid').value); \
  updateValue('wifi_passphrase', document.getElementById('wifi_passphrase').value); \
  updateValue('blynk_server', document.getElementById('blynk_server').value); \
  updateValue('blynk_port', document.getElementById('blynk_port').value); \
  updateValue('blynk_token', document.getElementById('blynk_token').value); \
  alert('Updated Configurations'); \
} \
</script> \
</body> \
</html>";

EasyBlynk8266Class::EasyBlynk8266Class(BlynkArduinoClient& transp) : Base(transp) {
    EEPROM.begin(512);
    EEPROM.get(0, config);

    if (strncmp(config.header, "ESP8266", 7) != 0) {
        // doesn't have any configuration
        strcpy(config.header, "ESP8266");
        strcpy(config.wifi_ssid, "not_configured_yet");
        strcpy(config.wifi_passphrase, "not_configured_yet");
        strcpy(config.blynk_server, "not_configured_yet");
        config.blynk_port = 0;
        strcpy(config.blynk_token, "not_configured_yet");

        EEPROM.put(0, config);
        EEPROM.commit();
    }
}

void EasyBlynk8266Class::begin() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    Base::begin(config.blynk_token);
    conn.begin(config.blynk_server, config.blynk_port);

    if (connectToWifi(30000) && connect()) {
        flash(3);
    } else {
        // failed to connect to Blynk server, will start configuration mode
        startConfigurationMode();
    }
}

void EasyBlynk8266Class::run() {
    if (configuration_mode) {
        server.handleClient();
        configuration_mode_led_timer.run();

        return;
    }

    Base::run();
}

void EasyBlynk8266Class::flash(int n) {
    for (int i = 0; i < n; i++) {
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
        digitalWrite(LED_BUILTIN, HIGH);
    }
}

boolean EasyBlynk8266Class::connectToWifi(int timeout) {
    int sleep_time = 100;

    WiFi.mode(WIFI_STA);

    if (config.wifi_passphrase && strlen(config.wifi_passphrase)) {
        WiFi.begin(config.wifi_ssid, config.wifi_passphrase);
    } else {
        WiFi.begin(config.wifi_ssid);
    }

    while (WiFi.status() != WL_CONNECTED && 0 < timeout) {
        delay(sleep_time);
        timeout -= sleep_time;
    }

    return WiFi.status() == WL_CONNECTED;
}

void EasyBlynk8266Class::startConfigurationMode() {
    IPAddress apIP(192, 168, 1, 1);

    WiFi.mode(WIFI_AP);
    WiFi.softAP("EasyBlynk8266-Config");
    delay(100); // ref: https://github.com/espressif/arduino-esp32/issues/985#issuecomment-359157428
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

    server.on("/", []() {
        EasyBlynk8266.handleRequest();
    });
    server.begin();

    configuration_mode_led_timer.setInterval(1000L, []() {
        EasyBlynk8266.flash(1);
    });
    configuration_mode = true;
}

void EasyBlynk8266Class::handleRequest() {
    String key = server.arg("key");
    String value = server.arg("value");

    if (key == "" && value == "") {
        String result = root_html_template;

        result.replace("[[wifi_ssid]]", config.wifi_ssid);
        result.replace("[[wifi_passphrase]]", config.wifi_passphrase);
        result.replace("[[blynk_server]]", config.blynk_server);
        result.replace("[[blynk_port]]", String(config.blynk_port));
        result.replace("[[blynk_token]]", config.blynk_token);

        server.send(200, "text/html", result);

        return;
    }

    if (key == "wifi_ssid") {
        strcpy(config.wifi_ssid, value.c_str());
    }

    if (key == "wifi_passphrase") {
        strcpy(config.wifi_passphrase, value.c_str());
    }

    if (key == "blynk_server") {
        strcpy(config.blynk_server, value.c_str());
    }

    if (key == "blynk_port") {
        config.blynk_port = value.toInt();
    }

    if (key == "blynk_token") {
        strcpy(config.blynk_token, value.c_str());
    }

    EEPROM.put(0, config);
    EEPROM.commit();

    server.send(200, "text/html", "OK");
};

WiFiClient _blynk_wifi_client;
BlynkArduinoClient _blynk_transport(_blynk_wifi_client);
EasyBlynk8266Class EasyBlynk8266(_blynk_transport);
