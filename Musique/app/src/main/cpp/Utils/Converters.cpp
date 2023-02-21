#include "Converters.h"

std::string ToString(int value)
{
    char* cstring = new char('0');
    sprintf(cstring, "%d", value);
    std::string string = cstring;
    delete cstring;
    return string;
}

std::string ToString(unsigned int value)
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

int ToInt(const char value)
{
    return int(value) - 48; // 48 is the ascii code for 0
}

unsigned int ToUnsignedInt(const std::string& value)
{
    return std::atoi(value.c_str());
}

double ToDouble(const std::string& value)
{
    //return (float)std::strtod(value.c_str(), nullptr);
    return std::atof(value.c_str());
}

float ToFloat(const std::string& value)
{
    return (float)ToDouble(value);
}

// ---- Is Value Functions ----

bool IsInt(const std::string& value)
{
    for (char c : value)
    {
        if (isdigit(c) || c == '-') // c is a digit or a negative sign '-'
            continue;
        else
            return false;
    }

    return true;
}

bool IsUnsignedInt(const std::string& value)
{
    for (char c : value)
    {
        if (isdigit(c)) // c is a digit (and not a negative sign '-')
            continue;
        else
            return false;
    }

    return true;
}

/*bool IsFloat(const std::string& value)
{
    bool hasDot = false;

    for (int i = 0; i < value.length(); i++) // random crashes here????
    {
        char c = value[i];
        if (isdigit(c) || c == '-') // c is a digit or a negative sign '-'
            continue;
        else if (c == '.' && !hasDot) // c has only one decimal point
        {
            hasDot = true;
            continue;
        }
        else
            return false;
    }
    return true; // is a float
}*/

// ---- Comparison Functions ----

bool FloatsAreEqual(float x, float y, float limit)
{
    return std::abs(x - y) < limit;
}