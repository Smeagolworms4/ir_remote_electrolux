#ifndef mqtt_MqttService_h
#define mqtt_MqttService_h

#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <helper.h>
#include <WebServer.h>
#include <FileFS.h>
#include "MqttConfig.h"

typedef std::function<void()> OnMqttCallback;

class MqttService {
    public:
        static const int SERVER_HANDLER_SIZE = 4;
        
        MqttService();
        virtual ~MqttService();
        String getApSSID();
        void init(FileFS *fileFS, WebServer *webserver, OnMqttCallback onConnect);
        void loop();

        MqttConfig* getConfig();
        void saveConfig();
        void loadConfig();
        void clearConfig();
        void stop();
        void restart();
        AsyncMqttClient* getClient();
    private:
        uint64 previous;
        AsyncMqttClient client;
        OnMqttCallback onConnect;
        FileFS* fileFS;
        WebServer* webServer;
        AsyncCallbackWebHandler* webServerHandlers[MqttService::SERVER_HANDLER_SIZE];
        MqttConfig config;
        String mqttStatus;
        bool running;
        bool connected;

        void clean();
        void initWebServer();
};

#endif