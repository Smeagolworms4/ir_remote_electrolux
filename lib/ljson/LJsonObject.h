#ifndef ljson_LJsonObject_h
#define ljson_LJsonObject_h

#include "LJsonNode.h"
#include "LJsonScalar.h"
#include "map"

class LJsonArray;

class LJsonObject : public LJsonNode {
    public:
        LJsonObject();
        virtual ~LJsonObject();
        LJsonObject* addChild(String key, String child);
        LJsonObject* addChild(String key, int child);
        LJsonObject* addChild(String key, long child);
        LJsonObject* addChild(String key, unsigned int child);
        LJsonObject* addChild(String key, unsigned long child);
        LJsonObject* addChild(String key, float child);
        LJsonObject* addChild(String key, double child);
        LJsonObject* addChild(String key, bool child);
        LJsonObject* addChild(String key, const char* child);
        LJsonObject* addChild(String key, LJsonNode* child);

        std::vector<String> getKeys();
        std::vector<LJsonNode*> getValues();
        bool hasChild(String key);
        LJsonNode* getChild(String key);
        LJsonObject* getChildObject(String key);
        LJsonArray*  getChildArray(String key);
        LJsonScalar* getChildScalar(String key);
        LJsonScalar* getChildNumber(String key);
        LJsonScalar* getChildNumberInt(String key);
        LJsonScalar* getChildNumberDec(String key);
        LJsonScalar* getChildString(String key);
        LJsonScalar* getChildBool(String key);

        virtual String toString();
        virtual LJsonType getType();
        virtual LJsonObject* clone();

    private:
        std::map<String, LJsonNode*>  children;
};

#endif