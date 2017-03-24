#ifndef GIFAUTHOR_EXCEPTIONS_H
#define GIFAUTHOR_EXCEPTIONS_H

#include <stdexcept>
#include <string>
#include <sstream>
#include <typeinfo>

using namespace std;

class CLineError: public std::runtime_error
{
    public:
        CLineError();
        virtual const char* what() const noexcept;

    protected:
        static ostringstream oss;
};

ostringstream CLineError::oss;

/**
 * When the incorrect type is given for an argument
 */
template <class T>
class ParseError: public CLineError
{
    public:
        ParseError(const string& arg, const string& src, const T& target):
            arg(arg),
            src(src)
        {};
        virtual const char* what() const noexcept
        {
            oss.str("");
            oss << "While parsing "<< arg 
                << ", couldn't interperet \"" << src << "\" as " 
                << typeid(T).name();
            return oss.str().c_str();
        };
    private:
        string arg, src;
};

/**
 * When an argument is unexpected
 */
class UnknownArgument: public CLineError
{
    public:
        UnknownArgument(const string& arg);
        virtual const char* what() const noexcept;
    private:
        string arg;
};


#endif //GIFAUTHOR_EXCEPTIONS_H