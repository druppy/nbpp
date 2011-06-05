//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/RegexResultImpl.hpp>
#include <nb++/Assert.hpp>

namespace nbpp
{
    RegexResultImpl::RegexResultImpl(bool arg_exprMatched, int arg_numSubs,
                                     regmatch_t* arg_regMatches) :
        exprMatched(arg_exprMatched), numSubs(arg_numSubs), regMatches(arg_regMatches) { }

    RegexResultImpl::~RegexResultImpl() throw()
    {
        delete[] regMatches;
    }

    bool RegexResultImpl::matched() const throw(AssertException, exception)
    {
        return exprMatched;
    }

    int RegexResultImpl::start() const throw(AssertException, exception)
    {
        if (exprMatched)
        {
            return regMatches[0].rm_so;
        }
        else
        {
            return 0;
        }
    }

    int RegexResultImpl::length() const throw(AssertException, exception)
    {
        if (exprMatched)
        {
            return regMatches[0].rm_eo - regMatches[0].rm_so;
        }
        else
        {
            return 0;
        }
    }

	Strings RegexResultImpl::getGroups( const string &str ) const 
	{
		Strings strs;

		for( int i = 1; i <= getSubExprCount(); i++ ) {
			RegexSubResult res = (*this)[ i ];

			strs.push_back( str.substr( res.start(), res.length()));
		}
		return strs;
	}

    RegexSubResult RegexResultImpl::operator [](int index) const
        throw(AssertException, exception){
        Assert(index > 0 && index < numSubs,
               AssertException("RegexSubResult index out of range"));

        regmatch_t* regMatch = &regMatches[index];

        if (regMatch->rm_so == -1)
        {
            return RegexSubResult(false, 0, 0);
        }
        else
        {
            return RegexSubResult(true, regMatch->rm_so,
                                  regMatch->rm_eo - regMatch->rm_so);
        }
    }

    int RegexResultImpl::getSubExprCount() const throw(AssertException, exception)
    {
        return numSubs - 1;
    }
}
