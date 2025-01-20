#include "LJsonScalar.h"

LJsonScalar::LJsonScalar() {
    this->type = NULL_VALUE;
    this->valueStr = "";
    this->valueInt = 0;
    this->valueDec = 0;
    this->valueBool = false;
}
LJsonScalar::LJsonScalar(String value       ): LJsonScalar() { this->setValue(value); }
LJsonScalar::LJsonScalar(int value          ): LJsonScalar() { this->setValue(value); }
LJsonScalar::LJsonScalar(long value         ): LJsonScalar() { this->setValue(value); }
LJsonScalar::LJsonScalar(unsigned int value ): LJsonScalar() { this->setValue(value); }
LJsonScalar::LJsonScalar(unsigned long value): LJsonScalar() { this->setValue(value); }
LJsonScalar::LJsonScalar(float value        ): LJsonScalar() { this->setValue(value); }
LJsonScalar::LJsonScalar(double value       ): LJsonScalar() { this->setValue(value); }
LJsonScalar::LJsonScalar(bool value         ): LJsonScalar() { this->setValue(value); }
LJsonScalar::LJsonScalar(const char* value        ): LJsonScalar() { this->setValue(value); }
LJsonScalar::~LJsonScalar() {}

void LJsonScalar::setValue()                    { this->type = NULL_VALUE;  }
void LJsonScalar::setValue(String value)        { this->type = STRING_VALUE; this->valueStr = value; }
void LJsonScalar::setValue(int value)           { this->type = INT_VALUE; this->valueInt = value; }
void LJsonScalar::setValue(long value)          { this->type = INT_VALUE; this->valueInt = value; }
void LJsonScalar::setValue(unsigned int value)  { this->type = UINT_VALUE; this->valueUInt = value; }
void LJsonScalar::setValue(unsigned long value) { this->type = UINT_VALUE; this->valueUInt = value; }
void LJsonScalar::setValue(float value)         { this->type = DEC_VALUE; this->valueDec = value; }
void LJsonScalar::setValue(double value)        { this->type = DEC_VALUE; this->valueDec = value; }
void LJsonScalar::setValue(bool value)          { this->type = BOOLEAN_VALUE; this->valueBool = value; }
void LJsonScalar::setValue(const char* value)         { this->type = STRING_VALUE; this->valueStr = String(value); }


String LJsonScalar::getValueStr() {
    switch (this->type) {
        case STRING_VALUE: return String(this->valueStr);
        case NULL_VALUE: return "null";
        case BOOLEAN_VALUE: return this->valueBool ? "true" : "false";
        case DEC_VALUE: return String(this->valueDec);
        case UINT_VALUE: return String(this->valueUInt);
        default: return String(this->valueInt);
    }
}

int LJsonScalar::getValueInt() {
    switch (this->type) {
        case STRING_VALUE: return std::atoi(this->valueStr.c_str());
        case NULL_VALUE: return 0;
        case BOOLEAN_VALUE: return this->valueBool ? 1 : 0;
        case DEC_VALUE: return (int)this->valueDec;
        case UINT_VALUE: return (int)(this->valueUInt);
        default: return (int)this->valueInt;
    }
}

long LJsonScalar::getValueLong() {
    switch (this->type) {
        case STRING_VALUE: return std::atol(this->valueStr.c_str());
        case NULL_VALUE: return 0L;
        case BOOLEAN_VALUE: return this->valueBool ? 1L : 0L;
        case DEC_VALUE: return (long)this->valueDec;
        case UINT_VALUE: return (long)(this->valueUInt);
        default: return this->valueInt;
    }
}

unsigned int LJsonScalar::getValueUInt() {
    switch (this->type) {
        case STRING_VALUE: return (unsigned int)std::atoi(this->valueStr.c_str());
        case NULL_VALUE: return 0;
        case BOOLEAN_VALUE: return (unsigned int)this->valueBool;
        case DEC_VALUE: return this->valueDec ? 1 : 0;
        case UINT_VALUE: return (unsigned int)(this->valueUInt);
        default: return (unsigned int)this->valueInt;
    }
}

unsigned long LJsonScalar::getValueULong() {
    switch (this->type) {
        case STRING_VALUE: return (unsigned long)std::atol(this->valueStr.c_str());
        case NULL_VALUE: return 0L;
        case BOOLEAN_VALUE: return this->valueBool ? 1L : 0L;
        case DEC_VALUE: return (unsigned long)this->valueDec;
        case UINT_VALUE: return this->valueUInt;
        default: return (unsigned long)this->valueInt;
    }
}

bool LJsonScalar::getValueBool() {
    String buf;
    switch (this->type) {
        case STRING_VALUE:
            buf = String(this->valueStr);
            buf.toLowerCase();
            return buf == "true";
        case NULL_VALUE: return false;
        case BOOLEAN_VALUE: return this->valueBool;
        case DEC_VALUE: return !!this->valueDec;
        case UINT_VALUE: return !!this->valueUInt;
        default: return !!this->valueInt;
    }
}

float LJsonScalar::getValueFloat() {
    switch (this->type) {
        case STRING_VALUE: return std::atof(this->valueStr.c_str());
        case NULL_VALUE: return 0.0F;
        case BOOLEAN_VALUE: return (float)this->valueBool;
        case DEC_VALUE: return (float)this->valueDec;
        case UINT_VALUE: return (float)this->valueUInt;
        default: return (float)this->valueInt;
    }
}

double LJsonScalar::getValueDouble() {
    char* endPtr;
    switch (this->type) {
        case STRING_VALUE: return std::strtod(this->valueStr.c_str(), &endPtr);
        case NULL_VALUE: return 0.0;
        case BOOLEAN_VALUE: return (double)this->valueBool;
        case DEC_VALUE: return this->valueDec;
        case UINT_VALUE: return (double)this->valueUInt;
        default: return (double)this->valueInt;
    }
}


String LJsonScalar::toString() {
    String result;
    switch (this->type) {
        case STRING_VALUE:
            result = String(this->valueStr);
            result.replace("\\", "\\\\");
            result.replace("\n", "\\n");
            result.replace("\r", "\\r");
            result.replace("\t", "\\t");
            result.replace("\"", "\\\"");
            return F("\"")+result+F("\"");

        case NULL_VALUE: return "null";
        case BOOLEAN_VALUE: return this->valueBool ? "true" : "false";
        case DEC_VALUE: return String(this->valueDec);
        case UINT_VALUE: return String(this->valueUInt);
        default: return String(this->valueInt);
    }
}

LJsonType LJsonScalar::getType() {
    return this->type;
}

LJsonScalar* LJsonScalar::clone() {
    LJsonScalar* clone = new LJsonScalar();
    clone->type = this->type;
    clone->valueStr = this->valueStr;
    clone->valueDec = this->valueDec;
    clone->valueInt = this->valueInt;
    clone->valueUInt = this->valueUInt;
    clone->valueBool = this->valueBool;
    return clone;
}