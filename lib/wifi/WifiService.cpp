#include "WifiService.h"

String mac2Str(const uint8 mac[6]) {
    char macStr[18] = { 0 };
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(macStr);
}

WifiService::WifiService(String name) {
    this->previous = millis();
    this->rescueMode = false;
    this->name = name;
    this->webServer = NULL;
    this->pinLedStatus = -1;
    this->scanRunning = false;
    this->config.apSsid = this->getApSSID();
    this->scanResults = new WiFiResultList();
    for (int i = 0; i < WifiService::SERVER_HANDLER_SIZE; i++) {
        this->webServerHandlers[i] = nullptr;
    }
}

WifiService::~WifiService() {
    this->clean();
    delete this->scanResults;
}

String WifiService::getApSSID() {
    String chipID = String(ESP.getChipId(), HEX);
    chipID.toUpperCase();
    return F("ESP8266_") + this->name + F("_") + chipID;
}

void WifiService::init(FileFS* fileFS) {
    this->init(fileFS, NULL, -1);
}


void WifiService::init(FileFS* fileFS, WebServer* webServer) {
    this->init(fileFS, webServer, -1);
}

void WifiService::init(FileFS* fileFS, WebServer* webServer, int pinLedStatus) {
    this->init(fileFS, webServer, pinLedStatus, false);
}

void WifiService::init(FileFS* fileFS, WebServer* webServer, int pinLedStatus, bool reverseFlash) {

    log_n("Initialize wifi service");
    log_l("    - MAC Address: "); log_n(WiFi.macAddress());

    this->fileFS = fileFS;
    this->webServer = webServer;
    this->pinLedStatus = pinLedStatus;
    this->reverseFlash = reverseFlash;
    this->fileFS = fileFS;
    
    this->clean();
    this->loadConfig();

    if (this->useLed()) {
        pinMode(this->pinLedStatus, OUTPUT);
        digitalWrite(this->pinLedStatus, this->reverseFlash ? HIGH : LOW);
    }

    this->scanAsync();
    this->initWebServer();
    this->startFromConfig();
}

void WifiService::setWifiMode() {
    if (this->config.apEnabled && this->config.stEnabled) {
        WiFi.mode(WIFI_AP_STA);
    } else if (this->config.apEnabled) {
        WiFi.mode(WIFI_AP);
    } else {
        WiFi.mode(WIFI_STA);
    }
}

void WifiService::startFromConfig() {

    WiFi.hostname(this->getApSSID());

    if (this->config.isValid()) {

        if (!this->rescueMode) {
            setWifiMode();
        }
                
        if (!this->rescueMode && this->config.apEnabled) { 
            log_n("Start wifi mode AP");
            log_l("    - SSID:     "); log_n(this->config.apSsid);
            log_l("    - Password: "); log_n(this->config.apPassword);

            if (this->config.apPassword) {
                WiFi.softAP(this->config.apSsid, this->config.apPassword);
            } else {
                WiFi.softAP(this->config.apSsid);
            }
        }
        if (this->config.stEnabled) {
            log_n("Start wifi mode Station");
            log_l("    - SSID:      "); log_n(this->config.stSsid);
            log_l("    - Password:  "); log_n(this->config.stPassword);
            log_l("    - DHCP Mode: "); log_n(WiFiConfig::dhcpMode2String(this->config.dhcp));
            log_l("    - Ip:        "); log_n(this->config.ip      != INADDR_ANY ? this->config.ip     .toString() : "");
            log_l("    - Mask:      "); log_n(this->config.mask    != INADDR_ANY ? this->config.mask   .toString() : "");
            log_l("    - Gateway:   "); log_n(this->config.gateway != INADDR_ANY ? this->config.gateway.toString() : "");
            log_l("    - Dns1:      "); log_n(this->config.dns1    != INADDR_ANY ? this->config.dns1   .toString() : "");
            log_l("    - Dns2:      "); log_n(this->config.dns2    != INADDR_ANY ? this->config.dns2   .toString() : "");

            WiFi.config(
                this->config.dhcp == STATIC && this->config.ip != INADDR_ANY ? this->config.ip : IPAddress(),
                this->config.dhcp == STATIC && this->config.mask != INADDR_ANY ? this->config.mask : IPAddress(),
                this->config.dhcp == STATIC && this->config.gateway != INADDR_ANY ? this->config.gateway : IPAddress(),
                this->config.dhcp != DHCP ? ( this->config.dns1 != INADDR_ANY ? this->config.dns1 : (this->config.dns2 ? this->config.dns2 : IPAddress()) ) : IPAddress(),
                this->config.dhcp != DHCP && this->config.dns1 != INADDR_ANY && this->config.dns2 != INADDR_ANY ? this->config.dns2 : IPAddress()
            );
            
            if (this->config.stPassword) {
                WiFi.begin(this->config.stSsid, this->config.stPassword);
            } else {
                WiFi.begin(this->config.stSsid);
            }
        }
    } else if(!this->rescueMode) {
        this->startRescueMode();
    }
}

void WifiService::startRescueMode() {
    log_n("Start wifi Rescue mode");
    log_l("    - SSID:    "); log_n(this->getApSSID());
    this->rescueMode = true;
    if (this->config.isValid() && this->config.stEnabled) {
        WiFi.mode(WIFI_AP_STA);
    } else {
        WiFi.mode(WIFI_STA);
    }
    WiFi.softAP(this->getApSSID());
    this->startFromConfig();
}

void WifiService::clean() {
    log_n("Clean Wifi Service");
    for (int i = 0; i < WifiService::SERVER_HANDLER_SIZE; i++) {
        if (this->webServerHandlers[i]) {
            this->webServer->getInstance()->removeHandler(this->webServerHandlers[i]);
            delete this->webServerHandlers[i];
            this->webServerHandlers[i] = nullptr;
        }
    }
    this->clearScanResults();
}

void WifiService::clearScanResults() {
    for (WiFiResult* r: *this->scanResults) {
        delete r;
    }
    this->scanResults->clear();
}

WiFiResultList* WifiService::scanAsync() {
    if (!this->scanRunning) {
        this->scanRunning = true;

        log_n("Wifi Scan start ... ");
        WiFi.scanNetworksAsync([this](int n) {
            this->clearScanResults();
            
            if (n > 0) {
                log_l("Wifi Scan network(s) found ("); log_l(n); log_n("): ");
                for (int i = 0; i < n; i++)
                {
                    WiFiResult* result = new WiFiResult(i); 
                    this->scanResults->push_back(result);
                    log_n("  - " + WiFi.SSID(i));
                }
                log_n();
            } else {
                log_l("Wifi Scan network(s) found (0)");
            }

            std::sort(this->scanResults->begin(), this->scanResults->end(), [](WiFiResult* a, WiFiResult* b) {
                return a->getQuality() > b->getQuality();
            });

            this->scanRunning = false;
        });
    }
    return this->scanResults;
}

void WifiService::initWebServer() {
    if (this->webServer) {
        
        LJsonAsyncWebServer *server = this->webServer->getInstance();
        log_n("Wifi init webserver");

        this->webServerHandlers[0] = &server->on("/api/wifi/status", HTTP_GET, [this] (AsyncWebServerRequest* request) {
            request->send(200, "application/json", ljson_stringify((new LJsonObject)
                ->addChild("mode", this->rescueMode ? String("RESCUE") : String("NORMAL"))
                ->addChild("ip", WiFi.localIP().toString())
                ->addChild("mask", WiFi.subnetMask().toString())
                ->addChild("gateway", WiFi.gatewayIP().toString())
                ->addChild("dns", WiFi.dnsIP().toString())
                ->addChild("quality", this->config.stEnabled ? (int)WiFiResult::rssi2Quality(WiFi.RSSI()) : 100)
            , true));
        });
        this->webServerHandlers[1] = &server->on("/api/wifi/scan", HTTP_GET, [this] (AsyncWebServerRequest* request) {
            WiFiResultList* list = this->scanAsync();
            request->send(200, "application/json", ljson_stringify((VectorLNormalize*) list));
        });
        this->webServerHandlers[2] = &server->on("/api/wifi/config", HTTP_GET, [this] (AsyncWebServerRequest* request) {
            request->send(200, "application/json", ljson_stringify(&this->config));
        });
        this->webServerHandlers[3] = &server->onJson<WiFiConfig>("/api/wifi/config", HTTP_POST, [this] (AsyncWebServerRequest* request, WiFiConfig* config) {
            this->config.copy(config);
            this->saveConfig();
            this->restartTime = millis() + 1000;
            request->send(200, "application/json", ljson_stringify(config, true));
        });
    }
}

WiFiConfig* WifiService::getConfig() {
    return &this->config;
}

void WifiService::saveConfig() {
    String str = ljson_stringify(&this->config);
    log_l("Save config: "); log_n(str);
    if (!this->fileFS->filePutContent("/wifi.json", str)) {
        log_n("Error on open wifi.json");
    }
}

void WifiService::loadConfig() {
    String str;
    log_l("Load config: ");
    if (this->fileFS->fileReadContent("/wifi.json", str)) {
        log_n(str);
        LJsonNode* json = ljson_parse(str);
        if (json) {
            if(!this->config.fromJson(json)) {
                this->config = WiFiConfig();
                this->config.apSsid = this->getApSSID();
                log_n("Error on parse json on /wifi.json");
            }
            delete json;
        } else {
            log_n("Error on parse json on /wifi.json");
        }
    } else {
        log_n("Error on open /wifi.json");
    }
}

void WifiService::clearConfig() {
    log_n("Clear config wifi");
    if (LittleFS.exists("/wifi.json")) {
        LittleFS.remove("/wifi.json");
    }
    this->config = WiFiConfig();
    this->config.apSsid = this->getApSSID();
    
}

bool WifiService::useLed() {
    return this->pinLedStatus != -1;
}

void WifiService::loop() {

    if (this->restartTime != 0 && this->restartTime < millis()) {
        this->restartTime = 0;
        this->rescueMode = false;
        this->startFromConfig();
    }
    
    if (this->useLed()) {

        if (this->rescueMode) {
            if (millis() - this->previous > 100) {
                this->previous = millis();
                if (digitalRead(this->pinLedStatus) == LOW) {
                    digitalWrite(this->pinLedStatus, HIGH);
                } else {
                    digitalWrite(this->pinLedStatus, LOW);
                }
            }
        } else
        if (WiFi.getMode() == WIFI_AP) {
            digitalWrite(this->pinLedStatus, this->reverseFlash ? LOW : HIGH);
        } else
        if (WiFi.isConnected()) {
            digitalWrite(this->pinLedStatus, this->reverseFlash ? LOW : HIGH);
        } else {
            if (millis() - this->previous > 700) {
                this->previous = millis();
                if (digitalRead(this->pinLedStatus) == LOW) {
                    digitalWrite(this->pinLedStatus, HIGH);
                } else {
                    digitalWrite(this->pinLedStatus, LOW);
                }
            }
        }
    }
}
