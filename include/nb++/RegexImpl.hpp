//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_REGEX_IMPL
#define _NBPP_REGEX_IMPL

#include <nb++/RegexResult.hpp>

namespace nbpp
{
    class RegexImpl : public RefCounted
    {
    public:
        RegexImpl(const string& pattern, int flags);

        ~RegexImpl() throw();
    
        RegexResult match(const string& text,
                          string::size_type offset,
                          int flags) const;

        RegexResult match(const char* text, int flags) const;

        int getSubExprCount() const;
    
    private:
        string getErrMsg(int errCode) const;
        regex_t reg;
    };
}

#endif /* _NBPP_REGEX_IMPL */
