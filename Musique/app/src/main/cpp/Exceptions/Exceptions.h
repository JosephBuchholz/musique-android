/**
 * This file is an include file for all exceptions.
 */

#ifndef MUSIQUE_EXCEPTIONS_H
#define MUSIQUE_EXCEPTIONS_H

#include "BaseException.h"

/**
 * This exception is thrown if an object does not exist.
 */
class DoesNotExistException : public BaseException
{
public:
    DoesNotExistException() : BaseException("Does not exist error has occurred.") {}
    DoesNotExistException(const std::string& msg) : BaseException(msg) {}
};

/**
 * This exception is thrown if an object is null.
 */
class IsNullException : public BaseException
{
public:
    IsNullException() : BaseException("Object is null.") {}
    IsNullException(const std::string& msg) : BaseException(msg) {}
};

/**
 * This exception is thrown if a container is empty when it should not be.
 */
class IsEmptyException : public BaseException
{
public:
    IsEmptyException() : BaseException("A container is empty.") {}
    IsEmptyException(const std::string& msg) : BaseException(msg) {}
};

#endif //MUSIQUE_EXCEPTIONS_H
