//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_REGEX_RESULT_IMPL
#define _NBPP_REGEX_RESULT_IMPL

#include <nb++/osdep.hpp>
#include <cstddef>

extern "C"
{
#include <regex.h>
}

#include "RefCount.hpp"
#include "Exception.hpp"
#include "RegexSubResult.hpp"
#include "String.hpp"

namespace nbpp
{
    class RegexResultImpl : public RefCounted
    {
    public:
        RegexResultImpl( bool exprMatched, int numSubs, regmatch_t* regMatches);
        ~RegexResultImpl() throw();

        bool matched() const throw(AssertException, exception);

        int start() const throw(AssertException, exception);

        int length() const throw(AssertException, exception);

		Strings getGroups( const string &str ) const;

        RegexSubResult operator [](int index) const throw(AssertException, exception);

        int getSubExprCount() const throw(AssertException, exception);

    private:
        bool exprMatched;
        int numSubs;
        regmatch_t* regMatches;
    };

    typedef RefHandle<RegexResultImpl> RegexResultHandle;
}

#endif /* _NBPP_REGEX_RESULT_IMPL */
