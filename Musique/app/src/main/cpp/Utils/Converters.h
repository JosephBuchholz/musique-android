#ifndef MUSIQUE_CONVERTERS_H
#define MUSIQUE_CONVERTERS_H

#include <string>

std::string ToString(int value);
std::string ToString(unsigned int value);
std::string ToString(float value);

/// converts a string value to an int value
int ToInt(const std::string& value);

/// converts a char value to an int value
int ToInt(const char value);

/// converts a string value to an unsigned int value
unsigned int ToUnsignedInt(const std::string& value);

/// converts a string value to a double value
double ToDouble(const std::string& value);

/// converts a string value to a float value
float ToFloat(const std::string& value);

// ---- Is Value Functions ----

bool IsInt(const std::string& value);
bool IsUnsignedInt(const std::string& value);

//bool IsFloat(const std::string& value);

// ---- Comparison Functions ----

/// finds if two floats are equal within a certain limit
bool FloatsAreEqual(float x, float y, float limit = 0.001f);

#endif // MUSIQUE_CONVERTERS_H