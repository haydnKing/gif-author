#include "exceptions.h"

ostringstream CLineError::oss;

CLineError::CLineError():
    runtime_error("Failed to parse command line")
{};

const char* CLineError::what() const throw()
{
    return str.c_str();
};


UnknownArgument::UnknownArgument(const string& arg)
{
    oss.str("");
    oss << "Unknown command line argument \"" << arg << "\"";
    str = oss.str();
};

