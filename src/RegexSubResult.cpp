//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/RegexSubResult.hpp>

namespace nbpp
{
    RegexSubResult::RegexSubResult(bool arg_exprMatched,
                                   int arg_startIndex, int arg_len)
        throw(AssertException, exception) :
        exprMatched(arg_exprMatched), startIndex(arg_startIndex),
        len(arg_len) { }

    RegexSubResult::~RegexSubResult() throw() { }

    bool RegexSubResult::matched() const throw(AssertException, exception)
    {
        return exprMatched;
    }

    int RegexSubResult::start() const throw(AssertException, exception)
    {
        return startIndex;
    }

    int RegexSubResult::length() const throw(AssertException, exception)
    {
        return len;
    }

    string RegexSubResult::substr(const string& text,
                                  string::size_type offset) const
        throw(AssertException, exception)
    {
        return matched()? text.substr(offset + start(), offset + length()) : "";
    }
}
