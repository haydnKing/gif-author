#include "exceptions.h"

CLineError::CLineError():
    runtime_error("")
{};

const char* CLineError::what() const noexcept
{
    return "Failed to parse command line";
};


UnknownArgument::UnknownArgument(const string& arg):
    arg(arg)
{};

const char* UnknownArgument::what() const noexcept
{
    oss.str("");
    oss << "Unknown command line argument \"" << arg << "\"";
    return oss.str().c_str();
};
