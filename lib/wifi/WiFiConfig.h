#ifndef wifi_WiFiConfig_h
#define wifi_WiFiConfig_h

#include "LJson.h"
#ifdef ESP8266
    #include <ESP8266WiFi.h>
#else //ESP32
    #include <WiFi.h>
#endif

enum WiFiConfigDHCPMode {
    DHCP,
    DHCP_DNS,
    STATIC
};

class WiFiConfig: public LNormalized, public LDenormalized {
    public:

        virtual ~WiFiConfig();

        static String dhcpMode2String(WiFiConfigDHCPMode mode);
        static WiFiConfigDHCPMode string2DHCPMode(String str);
        
        bool apEnabled = true;
        String apSsid = "";
        String apPassword = "";
        bool stEnabled = false;
        String stSsid = "";
        String stPassword = "";
        WiFiConfigDHCPMode dhcp = DHCP;
        IPAddress ip = INADDR_ANY;
        IPAddress mask = INADDR_ANY;
        IPAddress gateway = INADDR_ANY;
        IPAddress dns1 = INADDR_ANY;
        IPAddress dns2 = INADDR_ANY;

        bool isValid();
        virtual LJsonNode* toJson();
        virtual bool fromJson(LJsonNode* json);
        void copy(WiFiConfig* config);
};

#endif