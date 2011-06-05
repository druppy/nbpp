//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_REGEX_RESULT
#define _NBPP_REGEX_RESULT

#include <nb++/RegexResultImpl.hpp>
#include <nb++/String.hpp>

namespace nbpp
{
    /**
     * Represents the result of a Regex search.  This class is
     * reference-counted.  Instances of this class are constructed by
     * Regex.
     *
     * @see Regex
     * @see RegexSubResult
     */
    class RegexResult
    {
    public:
        RegexResult(bool matched, int numSubs, regmatch_t* regMatches)
            throw(AssertException, exception);

        ~RegexResult() throw();

        /**
         * @return true if a match was found.
         */
        bool matched() const throw(AssertException, exception);

        /**
         * @return the starting index of the string that matched the
         * pattern, or 0 if no match was found.
         */
        int start() const throw(AssertException, exception);

        /**
         * @return the length of the string that matched the pattern, or 0
         * if no match was found.
         */
        int length() const throw(AssertException, exception);

		/**
		   Return a string vector of all the results. This is the easy but also
		   somewhat more expensive version, as it allocates both a vector an some
		   new substrings. But its nice and easy :-)

		   @param the newly mached string
		   @return vector of result strings
		 */
		Strings getGroups( const string &str ) const;
 
        /**
         * @return a RegexSubResult representing the location of text
         * matching a subexpression.  The RegexSubResult objects with
         * indices 1..n give the locations of substrings that matched
         * subexpressions 1..n.
         *
         * @param index the 1-based index of the subexpression.
         * @exception AssertException if the index is out of range.
         */
        RegexSubResult operator [](int index) const
            throw(AssertException, exception);

        /**
         * @param text the string searched.
         * @param offset the index in the string at which the search started.
         * @return the substring that matched the
         * subexpression, or an empty string if there was no match
         * for the subexpression.
         */
        string substr(const string& text, string::size_type offset = 0) const
            throw(AssertException, exception);

        /**
         * Constructs a null RegexResult.
         */
        RegexResult() throw(exception);

        /**
         * @return true if this object is non-null.
         */
        operator bool() const throw();

        /**
         * @return the number of subexpressions in this result.
         */
        int getSubExprCount() const throw(AssertException, exception);

    private:
        RegexResultHandle impl;
    };
}

#endif /* _NBPP_REGEX_RESULT */
