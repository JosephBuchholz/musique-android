#include "Converters.h"

std::string ToString(int value)
{
    char* cstring = new char('0');
    sprintf(cstring, "%d", value);
    std::string string = cstring;
    delete cstring;
    return string;
}

std::string ToString(float value)
{
    //char* string = new char('0');
    //sprintf(string, "%f", value);
    char* cstring = new char('0');
    sprintf(cstring, "%f", value);
    std::string string = cstring;
    delete cstring;
    return string;
}

int ToInt(const std::string& value)
{
    return std::atoi(value.c_str());
}

unsigned int ToUnsignedInt(const std::string& value)
{
    return std::atoi(value.c_str());
}

float ToFloat(const std::string& value)
{
    return std::atof(value.c_str());
}

// ---- Is Value Functions ----

bool IsInt(const std::string& value)
{
    return true; // TODO: needs implemented
}

bool IsUnsignedInt(const std::string& value)
{
    return true; // TODO: needs implemented
}