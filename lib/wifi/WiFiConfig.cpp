#include "WiFiConfig.h"

String WiFiConfig::dhcpMode2String(WiFiConfigDHCPMode mode) {
    switch (mode) {
        case DHCP: return "DHCP";
        case DHCP_DNS: return "DHCP_DNS";
        case STATIC: return "STATIC";
        default: return "ERROR";
    }
}

WiFiConfigDHCPMode WiFiConfig::string2DHCPMode(String str) {
    if (str == "DHCP_DNS") return DHCP_DNS;
    if (str == "STATIC") return STATIC;
    return DHCP;
}

WiFiConfig::~WiFiConfig() {
}

bool WiFiConfig::isValid() {
    return (
        this->apEnabled && this->apSsid != ""
    ) ||
    (
        this->stEnabled && this->stSsid && (
            this->dhcp == DHCP ||
            this->dhcp == DHCP_DNS || (
                this->dhcp == STATIC &&
                this->ip &&
                this->mask
            )
        )
    );
}

LJsonNode* WiFiConfig::toJson() {
    return (new LJsonObject())
        ->addChild("stEnabled" , this->stEnabled)
        ->addChild("stSsid"    , this->stSsid)
        ->addChild("stPassword", this->stPassword)
        ->addChild("apEnabled" , this->apEnabled)
        ->addChild("apSsid"    , this->apSsid)
        ->addChild("apPassword", this->apPassword)
        ->addChild("dhcp"      , WiFiConfig::dhcpMode2String(this->dhcp))
        ->addChild("ip"        , this->ip      != INADDR_ANY ? this->ip     .toString() : "")
        ->addChild("mask"      , this->mask    != INADDR_ANY ? this->mask   .toString() : "")
        ->addChild("gateway"   , this->gateway != INADDR_ANY ? this->gateway.toString() : "")
        ->addChild("dns1"      , this->dns1    != INADDR_ANY ? this->dns1   .toString() : "")
        ->addChild("dns2"      , this->dns2    != INADDR_ANY ? this->dns2   .toString() : "")
    ;
}

bool jsonChildToIp(LJsonObject* obj, String key, IPAddress* ip) {
    if (LJsonScalar* c = obj->getChildString(key)) {
        String s = c->getValueStr();
        
        if (s != "" ) {
            if(!ip->fromString(s)) {
                ip->clear();
                return false;
            }
        }
        return true;
    }
    return false;
}

bool WiFiConfig::fromJson(LJsonNode* json) {
    if (!json->isObject()) return false;
    
    LJsonObject* obj = (LJsonObject*)json;
    if (LJsonScalar* c = obj->getChildBool("apEnabled"   )) { this->apEnabled  = c->getValueBool(); } else return false;
    if (LJsonScalar* c = obj->getChildString("apSsid"    )) { this->apSsid     = c->getValueStr(); } else return false;
    if (LJsonScalar* c = obj->getChildString("apPassword")) { this->apPassword = c->getValueStr(); } else return false;
    if (LJsonScalar* c = obj->getChildBool("stEnabled"   )) { this->stEnabled  = c->getValueBool(); } else return false;
    if (LJsonScalar* c = obj->getChildString("stSsid"    )) { this->stSsid     = c->getValueStr(); } else return false;
    if (LJsonScalar* c = obj->getChildString("stPassword")) { this->stPassword = c->getValueStr(); } else return false;
    
    if (LJsonScalar* c = obj->getChildString("dhcp"  )) { this->dhcp     = WiFiConfig::string2DHCPMode(c->getValueStr()); } else return false;
    if (!jsonChildToIp(obj, "ip"     , &this->ip     )) { return false; }
    if (!jsonChildToIp(obj, "mask"   , &this->mask   )) { return false; }
    if (!jsonChildToIp(obj, "gateway", &this->gateway)) { return false; }
    if (!jsonChildToIp(obj, "dns1"   , &this->dns1   )) { return false; }
    if (!jsonChildToIp(obj, "dns2"   , &this->dns2   )) { return false; }
    
    return this->isValid();
}


void WiFiConfig::copy(WiFiConfig* config) {
    this->stEnabled  = config-> stEnabled;
    this->stSsid     = config-> stSsid;
    this->stPassword = config-> stPassword;
    this->apEnabled  = config-> apEnabled;
    this->apSsid     = config-> apSsid;
    this->apPassword = config-> apPassword;
    this->dhcp       = config-> dhcp;
    this->ip         = config-> ip;
    this->mask       = config-> mask;
    this->gateway    = config-> gateway;
    this->dns1       = config-> dns1;
    this->dns2       = config-> dns2;
}