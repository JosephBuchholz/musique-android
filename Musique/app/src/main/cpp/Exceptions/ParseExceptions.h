/**
 * Contains exceptions for file parsing.
 */

#ifndef MUSIQUE_PARSEEXCEPTIONS_H
#define MUSIQUE_PARSEEXCEPTIONS_H


#include "BaseException.h"

/**
 * This exception is thrown if an object does not exist.
 */
class ParseException : public BaseException
{
public:
    ParseException() : BaseException("A error occurred while parsing.") {}
    ParseException(const std::string& msg) : BaseException(msg) {}
};

#endif //MUSIQUE_PARSEEXCEPTIONS_H
