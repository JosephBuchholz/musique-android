#pragma once

#include <string>

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
    return "0";
}

int ToInt(std::string value)
{
    return std::atoi(value.c_str());
}

float ToFloat(std::string value)
{
    return std::atof(value.c_str());
}