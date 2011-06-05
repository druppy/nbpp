//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/RegexImpl.hpp>
#include <nb++/Assert.hpp>

namespace nbpp
{
    RegexImpl::RegexImpl(const string& pattern, int flags)
    {
        int errCode = regcomp(&reg, pattern.c_str(),
                              flags | REG_EXTENDED);
        if (errCode)
        {
            string message = getErrMsg(errCode);
            regfree(&reg);
            throw RegexException(message);
        }
    }

    string RegexImpl::getErrMsg(int errCode) const
    {
        size_t errBufSize;
        errBufSize = regerror(errCode, &reg, NULL, 0);
        char errBuf[errBufSize];
        regerror(errCode, &reg, errBuf, errBufSize);
        return errBuf;
    }

    RegexImpl::~RegexImpl() throw()
    {
        regfree(&reg);
    }

    RegexResult RegexImpl::match(const string& text,
                                 string::size_type offset,
                                 int flags) const
    {
        Assert(offset >= 0 && offset <= text.size(),
               AssertException("Regex::match: String index out of range"));
    
        return match(text.c_str() + offset, flags);
    }

    RegexResult RegexImpl::match(const char* text, int flags) const
    {
        int numSubs = reg.re_nsub + 1;
        regmatch_t* regMatches = new regmatch_t[numSubs];
        int result = regexec(&reg, text, numSubs, regMatches, 0);
        switch (result)
        {
        case 0:
            return RegexResult( true, numSubs, regMatches);

        case REG_NOMATCH:
            return RegexResult( false, numSubs, regMatches);

        default:
            throw RegexException(getErrMsg(result));
        }
    }

    int RegexImpl::getSubExprCount() const 
    {
        return reg.re_nsub;
    }
}
