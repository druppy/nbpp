//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_REGEX
#define _NBPP_REGEX

#include <nb++/RegexImpl.hpp>

namespace nbpp
{
	using namespace std;
    /**
     * Represents a POSIX extended regular expression.  A Regex object
     * does not save the results of searches; instead, it returns a
     * RegexResult object for each search.  One Regex object can therefore
     * be used by multiple threads.  This class is reference-counted.
     *
     * @see RegexResult
     * @see RegexSubResult
     */
    class Regex
    {
    public:
        /**
         * Compiles a regular expression.
         *
         * @param pattern the pattern to compile.
         * @param flags the bitwise OR of any of the following:
         * REG_ICASE (ignore case), REG_NEWLINE (match-any-character operators 
         * don't match a newline; '^' and '$' match at newlines).
         *
         * @exception RegexException if there is a compilation error.
         */
        Regex(const string& pattern, int flags = 0);

        ~Regex() throw();

        /**
         * Looks for a match in a string.
         *
         * @param text the string to search in.
         * @param offset the index in the string at which to start the search.
         * @param flags the bitwise OR of any of the following: REG_NOTBOL (the beginning
         * of the string will not match '^'), REG_NOTEOL (the end of the string will not
         * match '$').
         *
         * @exception RegexException if the regular expression engine runs out of memory.
         * @exception AssertException if the offset is out of bounds.
         */
        RegexResult match(const string& text,
                          string::size_type offset = 0,
                          int flags = 0) const;

        /**
         * Looks for a match in a C-style string.
         *
         * @param text the string to search in.
         * @param flags the bitwise OR of any of the following:
         * REG_NOTBOL (the beginning of the string will not match '^'),
         * REG_NOTEOL (the end of the string will not match '$').
         *
         * @exception RegexException RegexException if the regular expression engine runs
         * out of memory.
         */
        RegexResult match(const char* text, int flags = 0) const;

        /**
         * Constructs a null Regex.
         */
        Regex() throw(exception);

        /**
         * @return true if this object is non-null.
         */
        operator bool() const throw();

        /**
         * @return the number of subexpressions in the pattern.
         */
        int getSubExprCount() const;

    private:
        RefHandle<RegexImpl> impl;
    };

	/**
	   Splits a string into substrings, using delimiters that match the specified
	   regular expression.
     
	   @param text the string to be split.
	   @param delim the regular expression to be used for matching delimiters.
	   @return a Strings containing the substrings.
	   @exception RegexException if the regular expression engine runs out of memory.
     */
    Strings split( const string& text, const Regex &delim );
}

#endif /* _NBPP_REGEX */
