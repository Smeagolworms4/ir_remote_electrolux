#ifndef mqtt_MqttConfig_h
#define mqtt_MqttConfig_h

#include "LJson.h"

class MqttConfig: public LNormalized, public LDenormalized {
    public:
        bool enable = false;
        String haDiscovery = "homeassistant";
        String host = "";
        String prefix = "";
        int port = 1883;
        String login = "";
        String password = "";

        virtual LJsonNode* toJson();
        virtual bool fromJson(LJsonNode* json);
        void copy(MqttConfig* config);
};

#endif