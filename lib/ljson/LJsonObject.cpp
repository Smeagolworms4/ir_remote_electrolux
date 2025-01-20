#include "LJsonObject.h"
#include "LJsonArray.h"

LJsonObject::LJsonObject() {}

LJsonObject::~LJsonObject() {
    for (std::pair<String, LJsonNode*> entry: this->children) {
        delete entry.second;
    }
    this->children.clear();
}

LJsonObject* LJsonObject::addChild(String key, String child) { return this->addChild(key, new LJsonScalar(child)); }
LJsonObject* LJsonObject::addChild(String key, int child   ) { return this->addChild(key, new LJsonScalar(child)); }
LJsonObject* LJsonObject::addChild(String key, long child  ) { return this->addChild(key, new LJsonScalar(child)); }
LJsonObject* LJsonObject::addChild(String key, unsigned int child   ) { return this->addChild(key, new LJsonScalar(child)); }
LJsonObject* LJsonObject::addChild(String key, unsigned long child  ) { return this->addChild(key, new LJsonScalar(child)); }
LJsonObject* LJsonObject::addChild(String key, float child ) { return this->addChild(key, new LJsonScalar(child)); }
LJsonObject* LJsonObject::addChild(String key, double child) { return this->addChild(key, new LJsonScalar(child)); }
LJsonObject* LJsonObject::addChild(String key, bool child  ) { return this->addChild(key, new LJsonScalar(child)); }
LJsonObject* LJsonObject::addChild(String key, const char* child  ) { return this->addChild(key, new LJsonScalar(child)); }

LJsonObject* LJsonObject::addChild(String key, LJsonNode* child) {
    this->children.insert(std::make_pair(key, child));
    return this;
}

std::vector<String> LJsonObject::getKeys() {
    std::vector<String> keys;
    for (const auto& entry : this->children) {
        keys.push_back(entry.first);
    }
    return keys;
}

std::vector<LJsonNode*> LJsonObject::getValues() {
    std::vector<LJsonNode*> values;
    for (const auto& entry : this->children) {
        values.push_back(entry.second);
    }
    return values;
}

bool LJsonObject::hasChild(String key) {
    return !!this->children.count(key);
}

LJsonNode* LJsonObject::getChild(String key) {
    return this->children.count(key) ? this->children.find(key)->second : NULL;
}

LJsonObject* LJsonObject::getChildObject   (String key) { LJsonNode* c = this->getChild(key); return c && c->isObject()    ? (LJsonObject*)c : NULL; }
LJsonArray*  LJsonObject::getChildArray    (String key) { LJsonNode* c = this->getChild(key); return c && c->isArray()     ? (LJsonArray* )c : NULL; }
LJsonScalar* LJsonObject::getChildScalar   (String key) { LJsonNode* c = this->getChild(key); return c && c->isScalar()    ? (LJsonScalar*)c : NULL; }
LJsonScalar* LJsonObject::getChildNumber   (String key) { LJsonNode* c = this->getChild(key); return c && c->isNumber()    ? (LJsonScalar*)c : NULL; }
LJsonScalar* LJsonObject::getChildNumberInt(String key) { LJsonNode* c = this->getChild(key); return c && c->isNumberInt() ? (LJsonScalar*)c : NULL; }
LJsonScalar* LJsonObject::getChildNumberDec(String key) { LJsonNode* c = this->getChild(key); return c && c->isNumberDec() ? (LJsonScalar*)c : NULL; }
LJsonScalar* LJsonObject::getChildString   (String key) { LJsonNode* c = this->getChild(key); return c && c->isString()    ? (LJsonScalar*)c : NULL; }
LJsonScalar* LJsonObject::getChildBool     (String key) { LJsonNode* c = this->getChild(key); return c && c->isBool()      ? (LJsonScalar*)c : NULL; }


String LJsonObject::toString() {
    String content;
    long i = 0;
    for (const auto& entry: this->children) {
        content += (i == 0 ? "" : ",") + LJsonScalar(entry.first).toString()+":"+entry.second->toString();
        i++;
    }
    return F("{") + content + F("}");
}

LJsonType LJsonObject::getType() {
    return OBJECT_VALUE;
}

LJsonObject* LJsonObject::clone() {
    LJsonObject* clone = new LJsonObject();
    for (const auto& entry: this->children) {
        clone->addChild(entry.first, entry.second->clone());
    }
    return clone;
}