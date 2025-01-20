#include "MqttService.h"

MqttService::MqttService() {
    this->previous = millis();
    this->fileFS = nullptr;
    this->webServer = nullptr;
    this->mqttStatus = "INITIALIZE";
    this->running = false;
    this->connected = false;
    for (int i = 0; i < MqttService::SERVER_HANDLER_SIZE; i++) {
        this->webServerHandlers[i] = nullptr;
    }
}

MqttService::~MqttService() {
    this->stop();
    this->clean();
}

void MqttService::init(FileFS* fileFS, WebServer* webServer, OnMqttCallback onConnect) {

    log_n("Initialize mqtt service");
    
    this->clean();
    this->loadConfig();

    this->fileFS = fileFS;
    this->webServer = webServer;
    this->onConnect = onConnect;
    
    this->initWebServer();
    this->restart();
}

void MqttService::restart() {
    log_n("Restarting MQTT Service");
    this->running = false;
    if (this->config.enable && !this->config.host.isEmpty()) {
        log_n("MQTT is enabled, attempting to connect...");

        this->running = true;
        this->client.disconnect();
        this->connected = false;
        log_n("Connectint to MQTT:");
        log_l("    - host: \""); log_l(this->config.host); log_n("\"");
        log_l("    - port: "); log_n(this->config.port);
        
        this->client.setServer(this->config.host.c_str(), this->config.port);
        this->client.setKeepAlive(30);
        
        if (!this->config.login.equals("")) {
            log_l("    - login: \""); log_l(this->config.login); log_n("\"");
            log_l("    - password: \""); log_l(this->config.password); log_n("\"");
            this->client.setCredentials(this->config.login.c_str(), this->config.password.c_str());
        }
        
        this->client.onConnect([this](bool sessionPresent) {
            log_n("MQTT connected");
            this->mqttStatus = "CONNECTED";
            if (!this->connected) {
                this->onConnect();
            }
            this->connected = true;
        });
        this->client.onDisconnect([this](AsyncMqttClientDisconnectReason reason) {
            log_n("MQTT disconnected");

            switch (reason) {
                case AsyncMqttClientDisconnectReason::TCP_DISCONNECTED:
                    log_n("Reason: TCP Disconnected");
                    this->mqttStatus = "TCP_DISCONNECTED";
                    break;
                case AsyncMqttClientDisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION:
                    log_n("Reason: Unacceptable Protocol Version");
                    this->mqttStatus = "UNACCEPTABLE_PROTOCOL_VERSION";
                    break;
                case AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED:
                    log_n("Reason: Identifier Rejected");
                    this->mqttStatus = "IDENTIFIER_REJECTED";
                    break;
                case AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE:
                    log_n("Reason: Server Unavailable");
                    this->mqttStatus = "SERVER_UNAVAILABLE";
                    break;
                case AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS:
                    log_n("Reason: Malformed Credentials");
                    this->mqttStatus = "MALFORMED_CREDENTIALS";
                    break;
                case AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED:
                    log_n("Reason: Not Authorized");
                    this->mqttStatus = "NOT_AUTHORIZED";
                    break;
                default:
                    log_n("Reason: Unknown");
                    this->mqttStatus = "UNKNOWN_ERROR";
                    break;
            }
            this->connected = false;
        });
        this->mqttStatus = "CONNECTING";
        this->client.connect();
    } else {
        log_n("MQTT is not enabled or not configured");
        this->mqttStatus = "DISABLED";
    }
}

AsyncMqttClient* MqttService::getClient() {
    return &this->client;
}

void MqttService::clean() {
    log_n("Clean Mqtt Service");
    for (int i = 0; i < MqttService::SERVER_HANDLER_SIZE; i++) {
        if (this->webServerHandlers[i]) {
            this->webServer->getInstance()->removeHandler(this->webServerHandlers[i]);
            delete this->webServerHandlers[i];
            this->webServerHandlers[i] = nullptr;
        }
    }
}

void MqttService::initWebServer() {
    if (this->webServer)
    {
        LJsonAsyncWebServer *server = this->webServer->getInstance();
        log_n("Wifi init webserver");

        this->webServerHandlers[0] = &server->on("/api/mqtt/status", HTTP_GET, [this] (AsyncWebServerRequest* request) {
            request->send(200, "application/json", ljson_stringify((new LJsonObject)
                ->addChild("status", this->mqttStatus)
            , true));
        });
        this->webServerHandlers[2] = &server->on("/api/mqtt/config", HTTP_GET, [this] (AsyncWebServerRequest* request) {
            request->send(200, "application/json", ljson_stringify(&this->config));
        });
        this->webServerHandlers[3] = &server->onJson<MqttConfig>("/api/mqtt/config", HTTP_POST, [this] (AsyncWebServerRequest* request, MqttConfig* config) {
            this->config.copy(config);
            this->saveConfig();
            this->restart();
            request->send(200, "application/json", ljson_stringify(config, true));
        });
    }
}

MqttConfig* MqttService::getConfig() {
    return &this->config;
}

void MqttService::saveConfig() {
    String str = ljson_stringify(&this->config);
    log_l("Save config: "); log_n(str);
    if (!this->fileFS->filePutContent("/mqtt.json", str)) {
        log_n("Error on open mqtt.json");
    }
}

void MqttService::loadConfig() {
    String str;
    log_l("Load config: ");
    if (this->fileFS->fileReadContent("/mqtt.json", str)) {
        log_n(str);
        LJsonNode* json = ljson_parse(str);
        if (json) {
            if(!this->config.fromJson(json)) {
                this->config = MqttConfig();
                log_n("Error on parse json on /mqtt.json");
            }
            delete json;
        } else {
            log_n("Error on parse json on /mqtt.json");
        }
    } else {
        log_n("Error on open /mqtt.json");
    }
}

void MqttService::clearConfig() {
    log_n("Clear config MQTT");
    if (LittleFS.exists("/mqtt.json")) {
        LittleFS.remove("/mqtt.json");
    }
    this->config = MqttConfig();
}

void MqttService::stop() {
    log_n("Stopping MQTT Service");
    if (this->client.connected()) {
        this->client.disconnect();
    }
    this->mqttStatus = "STOPPED";
}


void MqttService::loop() {
    if (millis() - this->previous > 5000) {
        this->previous = millis();
        if (this->running && !this->connected) {
            log_n("Connecting timeout retry connect");
            this->restart();
        }
    }
}
