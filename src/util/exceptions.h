#ifndef GIFAUTHOR_EXCEPTIONS_H
#define GIFAUTHOR_EXCEPTIONS_H

#include <stdexcept>
#include <string>
#include <sstream>
#include <typeinfo>

#include <iostream>

using namespace std;

class CLineError: public std::runtime_error
{
    public:
        CLineError();
        virtual const char* what() const throw();

    protected:
        static ostringstream oss;
        string str;
};


/**
 * When the incorrect type is given for an argument
 */
template <class T>
class ParseError: public CLineError
{
    public:
        ParseError(const string& arg, const string& src, const T& target)
        {
            oss.str("");
            oss << "While parsing "<< arg 
                << ", couldn't interperet \"" << src << "\" as " 
                << typeid(T).name();
            str = oss.str();
        };
};

/**
 * When an argument is unexpected
 */
class UnknownArgument: public CLineError
{
    public:
        UnknownArgument(const string& arg);
};

/**
 * Invalid factory option
 */
class BadOption: public CLineError
{
    public:
        BadOption(const string& arg, const string& opt);
};

/**
 * Don't continue
 */
class StopCommand: public CLineError
{
    public:
        StopCommand() {}
        virtual const char* what() const throw()
        {
            return NULL;
        };

};


#endif //GIFAUTHOR_EXCEPTIONS_H
