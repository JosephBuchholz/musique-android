#pragma once

#include <string>

std::string ToString(int value);
std::string ToString(float value);

int ToInt(const std::string& value);
unsigned int ToUnsignedInt(const std::string& value);

float ToFloat(const std::string& value);

// ---- Is Value Functions ----

bool IsInt(const std::string& value);
bool IsUnsignedInt(const std::string& value);