//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/Regex.hpp>

namespace nbpp
{
    Regex::Regex() throw(exception) { }

    Regex::Regex(const string& pattern, int flags) :
        impl(new RegexImpl(pattern, flags)) { }

    Regex::~Regex() throw() { }

    RegexResult Regex::match(const string& text,
                             string::size_type offset,
                             int flags) const
    {
        return impl->match(text, offset, flags);
    }

    RegexResult Regex::match(const char* text, int flags) const
    {
        return impl->match(text, flags);
    }

    int Regex::getSubExprCount() const
    {
        return impl->getSubExprCount();
    }

    Regex::operator bool() const throw()
    {
        return impl;
    }

    Strings split(const string& text, const Regex& sep)
    {
        Strings vec;

        string::size_type pos = 0;
        for (RegexResult match; (match = sep.match(text, pos)).matched();
             pos += match.start() + match.length())
            vec.push_back(text.substr(pos, match.start()));
        vec.push_back(text.substr(pos));

        return vec;
    }
}
