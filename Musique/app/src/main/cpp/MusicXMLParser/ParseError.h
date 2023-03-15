#ifndef MUSIQUE_PARSEERROR_H
#define MUSIQUE_PARSEERROR_H

#include <string>
#include <vector>

enum class ErrorLevel {
    None = 0, Error, Warning, Fatal
};

/**
 * Represents a parsing error.
 */
class ParseError
{
public:

    ParseError() {}
    ParseError(std::string title, std::string desc, std::string parentName, ErrorLevel errorLevel)
            : title(title), desc(desc), parentName(parentName), errorLevel(errorLevel) {}

public:

    std::string parentName = "";
    std::string title = "";
    std::string desc = "";

    ErrorLevel errorLevel = ErrorLevel::Error;

    static void PrintErrors();
};

static std::vector<ParseError> m_Errors = std::vector<ParseError>();

#endif //MUSIQUE_PARSEERROR_H
