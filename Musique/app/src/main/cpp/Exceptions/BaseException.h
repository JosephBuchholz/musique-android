/**
 * This file contains the BaseException class.
 */

#ifndef MUSIQUE_BASEEXCEPTION_H
#define MUSIQUE_BASEEXCEPTION_H

#include <exception>
#include <string>

/**
 * The base exception class for all other exceptions.
 */
class BaseException : public std::exception
{
public:
    BaseException() : msg("An unknown error has occurred") {}
    BaseException(const std::string& msg) : msg(msg) {}

    virtual const char* what() const noexcept override { return msg.c_str(); }

private:
    std::string msg;
};

#endif //MUSIQUE_BASEEXCEPTION_H
