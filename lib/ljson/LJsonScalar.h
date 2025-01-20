#ifndef ljson_LJsonScalar_h
#define ljson_LJsonScalar_h

#include "LJsonNode.h"

class LJsonScalar : public LJsonNode {
    public:
        LJsonScalar();
        LJsonScalar(String value);
        LJsonScalar(int value    );
        LJsonScalar(long value   );
        LJsonScalar(unsigned int value );
        LJsonScalar(unsigned long value);
        LJsonScalar(float value  );
        LJsonScalar(double value );
        LJsonScalar(bool value   );
        LJsonScalar(const char* value   );
        virtual ~LJsonScalar();

        void setValue(); 
        void setValue(String value);
        void setValue(int value);
        void setValue(long value);
        void setValue(unsigned int value);
        void setValue(unsigned long value);
        void setValue(float value);
        void setValue(double value);
        void setValue(bool value);
        void setValue(const char* value);

        String getValueStr();
        int getValueInt();
        long getValueLong();
        unsigned int getValueUInt();
        unsigned long getValueULong();
        bool getValueBool();
        float getValueFloat();
        double getValueDouble();

        virtual String toString();
        virtual LJsonType getType();
        virtual LJsonScalar* clone();

    private:
        LJsonType type;
        String valueStr;
        long valueInt;
        unsigned long valueUInt;
        double valueDec;
        boolean valueBool;
};

#endif