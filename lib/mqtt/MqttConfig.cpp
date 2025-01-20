#include "MqttConfig.h"

LJsonNode* MqttConfig::toJson() {
    return (new LJsonObject())
        ->addChild("enable"     , this->enable)
        ->addChild("haDiscovery", this->haDiscovery)
        ->addChild("host"       , this->host)
        ->addChild("port"       , this->port)
        ->addChild("login"      , this->login)
        ->addChild("password"   , this->password)
    ;
}

bool MqttConfig::fromJson(LJsonNode* json) {
    if (!json->isObject()) return false;
    
    LJsonObject* obj = (LJsonObject*)json;
    if (LJsonScalar* c = obj->getChildBool("enable")       ) { this->enable      = c->getValueBool(); } else return false;
    if (LJsonScalar* c = obj->getChildString("haDiscovery")) { this->haDiscovery = c->getValueStr();  } else return false;
    if (LJsonScalar* c = obj->getChildString("host")       ) { this->host        = c->getValueStr();  } else return false;
    if (LJsonScalar* c = obj->getChildNumber("port")       ) { this->port        = c->getValueUInt(); } else return false;
    if (LJsonScalar* c = obj->getChildString("login")      ) { this->login       = c->getValueStr();  } else return false;
    if (LJsonScalar* c = obj->getChildString("password")   ) { this->password    = c->getValueStr();  } else return false;

    return true;
}

void MqttConfig::copy(MqttConfig* config) {
    this->enable      = config->enable; 
    this->haDiscovery = config->haDiscovery; 
    this->host        = config->host; 
    this->port        = config->port; 
    this->login       = config->login; 
    this->password    = config->password; 
}
