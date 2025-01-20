#ifndef WifiService_h
#define WifiService_h

#include <Arduino.h>
#include <helper.h>
#include <WebServer.h>
#include <FileFS.h>
#ifdef ESP8266
    #include <ESP8266WiFi.h>
#else //ESP32
    #include <WiFi.h>
#endif
#include "WiFiResult.h"
#include "WiFiConfig.h"

#define WiFiResultList std::vector<WiFiResult*>

class WifiService {
    public:
        static const int SERVER_HANDLER_SIZE = 4;
        
        WifiService(String name);
        ~WifiService();
        String getApSSID();
        void init(FileFS* fileFS);
        void init(FileFS *fileFS, WebServer *webserver);
        void init(FileFS *fileFS, WebServer *webserver, int pinLedStatus);
        void init(FileFS *fileFS, WebServer *webserver, int pinLedStatus, bool reverseFlash);
        void loop();
        void startRescueMode();
        bool useLed();
        WiFiResultList* scanAsync();

        void saveConfig();
        void loadConfig();
        void clearConfig();

    private:
        String name;
        FileFS* fileFS;
        WebServer* webServer;
        int pinLedStatus;
        bool reverseFlash;
        uint64 previous;
        bool rescueMode;
        AsyncCallbackWebHandler* webServerHandlers[WifiService::SERVER_HANDLER_SIZE];
        WiFiResultList* scanResults;
        bool scanRunning;
        WiFiConfig config;
        uint64 restartTime = 0;

        void setWifiMode();
        void clearScanResults();
        void clean();
        void initWebServer();
        void startFromConfig();
};

#endif