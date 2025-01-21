#include <Arduino.h>
#include <helper.h>
#include <Arduino.h>
#include <helper.h>
#include <FileFS.h>
#include <WebServer.h>
#include <WifiService.h>
#include <MqttService.h>

#include <IRremoteESP8266.h>
#include <IRsend.h>

#define PROJECT_NAME "REMOTE_HOTTE"

#define PIN_LED_WIFI PIN_D4
#define SEND_PIN PIN_D8
#define LONG_RESET_TIME 10000 


// based on https://pastebin.com/N6kG7Wu5
#define HOB_TO_HOOD_UNIT_MICROS     725
#define H2H_1   HOB_TO_HOOD_UNIT_MICROS
#define H2H_2   (HOB_TO_HOOD_UNIT_MICROS*2) // 1450
#define H2H_3   (HOB_TO_HOOD_UNIT_MICROS*3) // 2175
#define H2H_4   (HOB_TO_HOOD_UNIT_MICROS*4) // 2900
#define H2H_5   (HOB_TO_HOOD_UNIT_MICROS*5) // 3625

// First entry is the length of the raw command
const uint16_t Fan1[] { H2H_2, H2H_2, H2H_1, H2H_2, H2H_3, H2H_2, H2H_1, H2H_2, H2H_1, H2H_1, H2H_1, H2H_2, H2H_1, H2H_3, H2H_1 };
const uint16_t Fan2[] { H2H_2, H2H_2, H2H_1, H2H_4, H2H_1, H2H_3, H2H_5, H2H_3, H2H_3 };
const uint16_t Fan3[] { H2H_1, H2H_3, H2H_4, H2H_4, H2H_3, H2H_1, H2H_1, H2H_3, H2H_3 };
const uint16_t Fan4[] { H2H_2, H2H_3, H2H_2, H2H_1, H2H_2, H2H_3, H2H_2, H2H_2, H2H_1, H2H_3, H2H_1, H2H_1, H2H_2 };
const uint16_t FanOff[] { H2H_1, H2H_2, H2H_1, H2H_2, H2H_3, H2H_2, H2H_1, H2H_2, H2H_2, H2H_3, H2H_1, H2H_2, H2H_1, H2H_1, H2H_1 };
const uint16_t LightOn[] { H2H_1, H2H_2, H2H_1, H2H_1, H2H_2, H2H_1, H2H_1, H2H_2, H2H_1, H2H_1, H2H_2, H2H_4, H2H_1, H2H_1, H2H_1, H2H_1, H2H_2 };
const uint16_t LightOff[] { H2H_1, H2H_2, H2H_1, H2H_1, H2H_1, H2H_1, H2H_1, H2H_3, H2H_1, H2H_1, H2H_1, H2H_2, H2H_1, H2H_2, H2H_1, H2H_1, H2H_1 };


int lengthFan1     = sizeof(Fan1    ) / sizeof(Fan1[0]);
int lengthFan2     = sizeof(Fan2    ) / sizeof(Fan2[0]);
int lengthFan3     = sizeof(Fan3    ) / sizeof(Fan3[0]);
int lengthFan4     = sizeof(Fan4    ) / sizeof(Fan4[0]);
int lengthFanOff   = sizeof(FanOff  ) / sizeof(FanOff[0]);
int lengthLightOn  = sizeof(LightOn ) / sizeof(LightOn [0]);
int lengthLightOff = sizeof(LightOff) / sizeof(LightOff[0]);

FileFS fileFS;
WebServer webServer;
WifiService wifiService(PROJECT_NAME);
MqttService mqttService;
IRsend irsend(SEND_PIN);

struct {
    unsigned long duration;
    int resetCount;
} rtcMem;

unsigned long currentTime;
String lastSignal = "";
String mqttFanSpeed = "1";
uint8_t signal = 0;
uint previous = 0;
uint sendConfig = 0;

String deviceId() {
    return String(ESP.getChipId(), HEX);
}

String nameDeviceId() {
    return String(PROJECT_NAME) + F("_") + deviceId();
}

String devicePath() {
    String path = F("diy/") + nameDeviceId() + F("/");
    path.toLowerCase();
    return path;
}

String sendPath() {
    return devicePath() + F("send");   
}

String speedPath() {
    return devicePath() + F("speed");   
}

String statePath() {
    return devicePath() + F("state");
}

LJsonObject* mqttJsonDevice() {
    return (new LJsonObject())
        ->addChild("name", nameDeviceId())
        ->addChild("identifiers", (new LJsonArray())
            ->addChild(nameDeviceId())
        )
        ->addChild("manufacturer", "diy")
        ->addChild("configuration_url", String("http://")+WiFi.localIP().toString())
    ;
}

void sendState() {
    log_n("Send state to MQTT");
    int speed = 0;
    if (mqttFanSpeed == "1") speed = 1;
    if (mqttFanSpeed == "2") speed = 2;
    if (mqttFanSpeed == "3") speed = 3;
    if (mqttFanSpeed == "4") speed = 4;
    
    mqttService.getClient()->publish(statePath().c_str(), 1, true, ljson_stringify((new LJsonObject)
        ->addChild("last_signal", lastSignal)
        ->addChild("fan_speed", speed)
        ->addChild("fan_speed_percent", speed * 100 / 4)
        ->addChild("wifi_ip", WiFi.localIP().toString())
        ->addChild("wifi_signal", (int)WiFiResult::rssi2Quality(WiFi.RSSI()))
        ->addChild("wifi_ssid", wifiService.getConfig()->stSsid)
    , true).c_str());
}

void sendStateConfigSensorLastSignal() {
    String mqttPath = mqttService.getConfig()->haDiscovery;
    if (mqttPath != "") {
        log_n("Send discovery config sensor_last_signal to MQTT");
        
        String path = mqttPath + F("/sensor/diy/") + nameDeviceId() + F("_last_signal/config");
        path.toLowerCase();
        
        mqttService.getClient()->publish(path.c_str(), 1, true, ljson_stringify((new LJsonObject)
            ->addChild("state_topic", statePath())
            ->addChild("value_template", "{{ value_json.last_signal }}")
            ->addChild("name", "Last signal")
            ->addChild("unique_id", nameDeviceId() + F("_last_signal"))
            ->addChild("object_id", nameDeviceId() + F("_last_signal"))
            ->addChild("device", mqttJsonDevice())
        , true).c_str());
    }
}

void sendStateConfigSensorWifiIp() {
    String mqttPath = mqttService.getConfig()->haDiscovery;
    if (mqttPath != "") {
        log_n("Send discovery config sensor_wifi_ip to MQTT");
        
        String path = mqttPath + F("/sensor/diy/") + nameDeviceId() + F("_wifi_ip/config");
        path.toLowerCase();
        
        mqttService.getClient()->publish(path.c_str(), 1, true, ljson_stringify((new LJsonObject)
            ->addChild("state_topic", statePath())
            ->addChild("value_template", "{{ value_json.wifi_ip }}")
            ->addChild("entity_category", "diagnostic")
            ->addChild("qos", 0)
            ->addChild("name", "WiFi ip") 
            ->addChild("icon", "mdi:ip-outline") 
            ->addChild("unique_id", nameDeviceId() + F("_wifi_ip"))
            ->addChild("object_id", nameDeviceId() + F("_wifi_ip"))
            ->addChild("device", mqttJsonDevice())
        , true).c_str());
    }
}

void sendStateConfigSensorWifiSsid() {
    String mqttPath = mqttService.getConfig()->haDiscovery;
    if (mqttPath != "") {
        log_n("Send discovery config sensor_wifi_ssid to MQTT");
        
        String path = mqttPath + F("/sensor/diy/") + nameDeviceId() + F("_wifi_ssid/config");
        path.toLowerCase();
        
        mqttService.getClient()->publish(path.c_str(), 1, true, ljson_stringify((new LJsonObject)
            ->addChild("state_topic", statePath())
            ->addChild("value_template", "{{ value_json.wifi_ssid }}")
            ->addChild("entity_category", "diagnostic")
            ->addChild("qos", 0)
            ->addChild("name", "WiFi SSID") 
            ->addChild("icon", "mdi:wifi-settings") 
            ->addChild("unique_id", nameDeviceId() + F("wifi_ssid"))
            ->addChild("object_id", nameDeviceId() + F("wifi_ssid"))
            ->addChild("device", mqttJsonDevice())
        , true).c_str());
    }
}

void sendStateConfigSensorWifiSignal() {
    String mqttPath = mqttService.getConfig()->haDiscovery;
    if (mqttPath != "") {
        log_n("Send discovery config sensor_wifi_signal to MQTT");
        
        String path = mqttPath + F("/sensor/diy/") + nameDeviceId() + F("_wifi_signal/config");
        path.toLowerCase();
        
        mqttService.getClient()->publish(path.c_str(), 1, true, ljson_stringify((new LJsonObject)
            ->addChild("state_topic", statePath())
            ->addChild("value_template", "{{ value_json.wifi_signal }}")
            ->addChild("entity_category", "diagnostic")
            ->addChild("dev_cla", "signal_strength")
            ->addChild("unit_of_meas", "dBm")
            ->addChild("stat_cla", "measurement")
            ->addChild("qos", 0)
            ->addChild("name", "WiFi signal") 
            ->addChild("unique_id", nameDeviceId() + F("_wifi_signal"))
            ->addChild("object_id", nameDeviceId() + F("_wifi_signal"))
            ->addChild("device", mqttJsonDevice())
        , true).c_str());
    }
}

void sendStateConfigLight() {
    String mqttPath = mqttService.getConfig()->haDiscovery;
    if (mqttPath != "") {
        log_n("Send discovery config light to MQTT");
        
        String path = mqttPath + F("/light/diy/") + nameDeviceId() + F("_light/config");
        path.toLowerCase();
        
        mqttService.getClient()->publish(path.c_str(), 1, true, ljson_stringify((new LJsonObject)
            ->addChild("command_topic", sendPath())
            ->addChild("payload_on", "light/on")
            ->addChild("payload_off", "light/off")
            ->addChild("qos", 2)
            ->addChild("name", "Light")
            ->addChild("unique_id", nameDeviceId() + F("_light"))
            ->addChild("object_id", nameDeviceId() + F("_light"))
            ->addChild("device", mqttJsonDevice())
        , true).c_str());
    }
}

void sendStateConfigFan() {
    String mqttPath = mqttService.getConfig()->haDiscovery;
    if (mqttPath != "") {
        log_n("Send discovery config fan to MQTT");
        
        String path = mqttPath + F("/fan/diy/") + nameDeviceId() + F("_fan/config");
        path.toLowerCase();
        
        mqttService.getClient()->publish(path.c_str(), 1, true, ljson_stringify((new LJsonObject)
            ->addChild("percentage_state_topic", statePath())
            ->addChild("percentage_value_template", "{{ value_json.fan_speed }}")
            ->addChild("command_topic", sendPath())
            ->addChild("percentage_command_topic", speedPath())
            ->addChild("payload_on", "fan/on")
            ->addChild("payload_off", "fan/off")
            ->addChild("speed_range_min", 1)
            ->addChild("speed_range_max", 4)
            ->addChild("qos", 2)
            ->addChild("name", "Fan")
            ->addChild("unique_id", nameDeviceId() + F("_fan"))
            ->addChild("object_id", nameDeviceId() + F("_fan"))
            ->addChild("device", mqttJsonDevice())
        , true).c_str());
    }
}

void sendStateConfigFanBt(int number) {
    String mqttPath = mqttService.getConfig()->haDiscovery;
    if (mqttPath != "") {
        log_l("Send discovery config fan button "); log_l(number); log_n(" to MQTT");
        
        String path = mqttPath + F("/button/diy/") + nameDeviceId() + F("_fan_button_") + number + F("/config");
        path.toLowerCase();
        
        mqttService.getClient()->publish(path.c_str(), 1, true, ljson_stringify((new LJsonObject)
            ->addChild("command_topic", sendPath())
            ->addChild("payload_press", String("fan/") + number)
            ->addChild("qos", 2)
            ->addChild("name", String("Fan speed ") + number)
            ->addChild("unique_id", nameDeviceId() + F("_fan_") + number)
            ->addChild("object_id", nameDeviceId() + F("_fan_") + number)
            ->addChild("device", mqttJsonDevice())
        , true).c_str());
    }
}

void setup() {
    
    irsend.begin();
    log_init(115200);
    
    log_n(F("START " __FILE__ " from " __DATE__));

    fileFS.init([]() {
        // On error initialize FS
        pinMode(PIN_LED_WIFI, OUTPUT);
        while(1) {
            led_flash(PIN_LED_WIFI, 100, 1000);
        } 
    });
    
    // Lire la mémoire RTC
    currentTime = millis();
    system_rtc_mem_read(65, &rtcMem, sizeof(rtcMem));

    if (rtcMem.duration < LONG_RESET_TIME) {
        rtcMem.resetCount++;
    } else {
        rtcMem.resetCount = 1;
    }
    rtcMem.duration = 0;
    system_rtc_mem_write(65, &rtcMem, sizeof(rtcMem));

    if (rtcMem.resetCount >= 6) { 
        log_n("VERY MULTI PRESS RESET !");
        wifiService.clearConfig();
        mqttService.clearConfig();
        rtcMem.duration = 0;
        rtcMem.resetCount = 0;
        system_rtc_mem_write(65, &rtcMem, sizeof(rtcMem));
        log_n("RESTART !");
        ESP.restart();
        return;
    } else
    if (rtcMem.resetCount >= 3) {  // Exemple : 3 redémarrages successifs = appui long détecté
        log_n("multi press Reset !");
        wifiService.startRescueMode();
    } else {
        log_l("Reset normal ("); log_l(rtcMem.resetCount); log_n(")");
    }
    
    sendConfig = millis();
    
    webServer.init();
    wifiService.init(&fileFS, &webServer, PIN_LED_WIFI, true);
    
    mqttService.init(&fileFS, &webServer, [] () {
        mqttService.getClient()->subscribe(sendPath().c_str(), 1);
        mqttService.getClient()->subscribe(speedPath().c_str(), 1);
        sendState();
    });
    mqttService.getClient()->onMessage([] (char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
        String topicStr = String(topic);
        String payloadStr = String(payload);
        payloadStr = payloadStr.substring(0, len);
        payloadStr.trim();
        
        log_n("On message: ");
        log_l("    - name: ");log_n(topicStr);
        log_l("    - payload: "); log_n(payloadStr);
        
        if (topicStr == speedPath()) {
            mqttFanSpeed = payloadStr;
            log_l("Change preset to: "); log_n(mqttFanSpeed);
            if (mqttFanSpeed == "1") {
                signal = 4;
                lastSignal = "fan/1";
            } else if (mqttFanSpeed == "2") {
                signal = 5;
                lastSignal = "fan/2";
            } else if (mqttFanSpeed == "3") {
                signal = 6;
                lastSignal = "fan/3";
            } else if (mqttFanSpeed == "4") {
                signal = 7;
                lastSignal = "fan/4";
            } else {
                signal = 3;
                lastSignal = "fan/0";
            }
        } else
        if (topicStr == sendPath()) {
            if (payloadStr.startsWith("light/on")) {
                signal = 1;
                lastSignal = "light/on";
            }
            if (payloadStr.startsWith("light/off")) {
                signal = 2;
                lastSignal = "light/off";
            }
            if (payloadStr.startsWith("fan/off")) {
                signal = 3;
                lastSignal = "fan/0";
            }
            if (payloadStr.startsWith("fan/on")) {
                log_l("Set fan on with speed:"); log_n(mqttFanSpeed);
                if (mqttFanSpeed == "1") {
                    signal = 4;
                    lastSignal = "fan/1";
                } else if (mqttFanSpeed == "2") {
                    signal = 5;
                    lastSignal = "fan/2";
                } else if (mqttFanSpeed == "3") {
                    signal = 6;
                    lastSignal = "fan/3";
                } else if (mqttFanSpeed == "4") {
                    signal = 7;
                    lastSignal = "fan/4";
                } else {
                    signal = 3;
                    lastSignal = "fan/0";
                }
            }
            if (payloadStr.startsWith("fan/1")) {
                signal = 4;
                lastSignal = "fan/1";
            }
            if (payloadStr.startsWith("fan/2")) {
                signal = 5;
                lastSignal = "fan/2";
            }
            if (payloadStr.startsWith("fan/3")) {
                signal = 6;
                lastSignal = "fan/3";
            }
            if (payloadStr.startsWith("fan/4")) {
                signal = 7;
                lastSignal = "fan/4";
            }
        }
        
    });
    
    LJsonAsyncWebServer *server = webServer.getInstance();
    server->on("/api/hotte/last", HTTP_GET, [] (AsyncWebServerRequest* request) {
        request->send(200, "application/json", ljson_stringify((new LJsonObject)
            ->addChild("last",  lastSignal)
        , true));
    });
    server->on("/api/hotte/light/on", HTTP_GET, [] (AsyncWebServerRequest* request) {
        signal = 1;
        lastSignal = "light/on";
        request->send(200, "application/json", ljson_stringify((new LJsonObject)
            ->addChild("resul", true)
        , true));
    });
    server->on("/api/hotte/light/off", HTTP_GET, [] (AsyncWebServerRequest* request) {
        signal = 2;
        lastSignal = "light/off";
        request->send(200, "application/json", ljson_stringify((new LJsonObject)
            ->addChild("resul", true)
        , true));
    });
    server->on("/api/hotte/fan/0", HTTP_GET, [] (AsyncWebServerRequest* request) {
        signal = 3;
        lastSignal = "fan/0";
        request->send(200, "application/json", ljson_stringify((new LJsonObject)
            ->addChild("resul", true)
        , true));
    });
    server->on("/api/hotte/fan/1", HTTP_GET, [] (AsyncWebServerRequest* request) {
        signal = 4;
        lastSignal = "fan/1";
        request->send(200, "application/json", ljson_stringify((new LJsonObject)
            ->addChild("resul", true)
        , true));
    });
    server->on("/api/hotte/fan/2", HTTP_GET, [] (AsyncWebServerRequest* request) {
        signal = 5;
        lastSignal = "fan/2";
        request->send(200, "application/json", ljson_stringify((new LJsonObject)
            ->addChild("resul", true)
        , true));
    });
    server->on("/api/hotte/fan/3", HTTP_GET, [] (AsyncWebServerRequest* request) {
        signal = 6;
        lastSignal = "fan/3";
        request->send(200, "application/json", ljson_stringify((new LJsonObject)
            ->addChild("resul", true)
        , true));
    });
    server->on("/api/hotte/fan/4", HTTP_GET, [] (AsyncWebServerRequest* request) {
        signal = 7;
        lastSignal = "fan/4";
        request->send(200, "application/json", ljson_stringify((new LJsonObject)
            ->addChild("resul", true)
        , true));
    });
}

void send(const uint16_t signal[], uint16_t length) {
    irsend.sendRaw(signal, length, 38);
    delay(50); 
    irsend.sendRaw(signal, length, 38);
    delay(50); 
    irsend.sendRaw(signal, length, 38);
    delay(50); 
    irsend.sendRaw(signal, length, 38);
}

int stepConfig = 0;

void loop() {
    
    if (rtcMem.duration == 0) {
        if ((millis() - currentTime) >= LONG_RESET_TIME) {
            rtcMem.duration = LONG_RESET_TIME + 10;
            system_rtc_mem_write(65, &rtcMem, sizeof(rtcMem));
        }
    }
    
    if (signal == 1) {
        delay(500);
        log_n("Send Light on");
        send(LightOn, lengthLightOn);
        signal = 0;
        sendState();
        delay(500);
    }
    if (signal == 2) { 
        delay(500);
        log_n("Send Light off");
        send(LightOff, lengthLightOff);
        signal = 0;
        sendState();
        delay(500);
    }
    if (signal == 3) {
        delay(500);
        log_n("Send Fan off");
        send(FanOff, lengthFanOff);
        signal = 0;
        sendState();
        delay(500);
    }
    if (signal == 4) {
        delay(500);
        log_n("Send Fan 1");
        send(Fan1, lengthFan1);
        signal = 0;
        mqttFanSpeed = "1";
        sendState();
        delay(500);
    }
    if (signal == 5) {
        delay(500);
        log_n("Send Fan 2");
        send(Fan2, lengthFan2);
        signal = 0;
        mqttFanSpeed = "2";
        sendState();
        delay(500);
    }
    if (signal == 6) {
        delay(500);
        log_n("Send Fan 3");
        send(Fan3, lengthFan3);
        signal = 0;
        mqttFanSpeed = "3";
        sendState();
        delay(500);
    }
    if (signal == 7) {
        delay(500);
        log_n("Send Fan 4");
        send(Fan4, lengthFan4);
        signal = 0;
        mqttFanSpeed = "4";
        sendState();
        delay(500);
    }
    if (ESP.getFreeHeap() < 5000) {
        ESP.restart();
    }
    
    
    uint now = millis();
    
    if (now - sendConfig > 50000) {
        sendConfig = now;
        stepConfig = 0;
    } else
    if (now - sendConfig > 20000) {
        if (stepConfig == 9) {
            stepConfig++;
            sendStateConfigSensorWifiSsid();
        }
    } else
    if (now - sendConfig > 18000) {
        if (stepConfig == 8) {
            stepConfig++;
            sendStateConfigSensorWifiIp();
        }
    } else
    if (now - sendConfig > 16000) {
        if (stepConfig == 7) {
            stepConfig++;
            sendStateConfigSensorWifiSignal();
        }
    } else
    if (now - sendConfig > 14000) {
        if (stepConfig == 6) {
            stepConfig++;
            sendStateConfigFanBt(4);
        }
    } else
    if (now - sendConfig > 12000) {
        if (stepConfig == 5) {
            stepConfig++;
            sendStateConfigFanBt(3);
        }
    } else
    if (now - sendConfig > 10000) {
        if (stepConfig == 4) {
            stepConfig++;
            sendStateConfigFanBt(2);
        }
    } else
    if (now - sendConfig > 8000) {
        if (stepConfig == 3) {
            stepConfig++;
            sendStateConfigFanBt(1);
        }
    } else
    if (now - sendConfig > 6000) {
        if (stepConfig == 2) {
            stepConfig++;
            sendStateConfigFan();
        }
    } else
    if (now - sendConfig > 4000) {
        if (stepConfig == 1) {
            stepConfig++;
            sendStateConfigLight();
        }
    } else
    if (now - sendConfig > 2000) {
        if (stepConfig == 0) {
            stepConfig++;
            sendStateConfigSensorLastSignal();
        }
    }
    
    
    if (now - previous > 2000) {
        previous = now;
        log_l("MEMORY: ");
        log_l(ESP.getFreeHeap());
        log_l(" MFB: ");
        log_l(ESP.getMaxFreeBlockSize());
        log_l(" FRAG: ");
        log_n(ESP.getHeapFragmentation());
    }
   
    fileFS.loop();
    webServer.loop();
    wifiService.loop();
    mqttService.loop();
}
