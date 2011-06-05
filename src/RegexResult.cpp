//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/RegexResult.hpp>

namespace nbpp
{
    RegexResult::RegexResult( bool exprMatched, int numSubs, regmatch_t*
                             regMatches) throw(AssertException, exception) :
        impl(new RegexResultImpl( exprMatched, numSubs, regMatches)) { }
    
    RegexResult::RegexResult() throw(exception) { }

    RegexResult::~RegexResult() throw() { }

    bool RegexResult::matched() const throw(AssertException, exception)
    {
        return impl->matched();
    }

    int RegexResult::start() const throw(AssertException, exception)
    {
        return impl->start();
    }

    int RegexResult::length() const throw(AssertException, exception)
    {
        return impl->length();
    }

	Strings RegexResult::getGroups( const string &str ) const 
	{
		return impl->getGroups( str );
	}

    RegexSubResult RegexResult::operator [](int index) const
        throw(AssertException, exception)
    {
        return (*impl)[index];
    }

    string RegexResult::substr(const string& text, string::size_type offset) const
        throw(AssertException, exception)
    {
        return matched() ? text.substr(offset + start(), offset + length()) : "";
    }

    RegexResult::operator bool() const throw()
    {
        return impl;
    }

    int RegexResult::getSubExprCount() const throw(AssertException, exception)
    {
        return impl->getSubExprCount();
    }
}
