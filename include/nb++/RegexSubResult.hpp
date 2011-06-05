//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_REGEX_SUB_RESULT
#define _NBPP_REGEX_SUB_RESULT

#include <nb++/osdep.hpp>
#include <nb++/Exception.hpp>

#include <string>

namespace nbpp
{
    using std::string;

    /**
     * Represents a substring matched by a subexpression in a Regex.  This
     * class is not reference-counted, but is inexpensive to pass by
     * value.
     *
     * @see Regex
     * @see RegexResult
     */
    class RegexSubResult
    {
    public:
        RegexSubResult(bool exprMatched, int startIndex, int len)
            throw(AssertException, exception);

        ~RegexSubResult() throw();

        /**
         * @return true if there was a match for the subexpression.
         */
        bool matched() const throw(AssertException, exception);

        /**
         * @return the starting index of the substring that matched the
         * subexpression, or 0 if there was no match for the
         * subexpression.
         */
        int start() const throw(AssertException, exception);

        /**
         * @return the length of the substring that matched the
         * subexpression, or 0 if there was no match for the
         * subexpression.
         */
        int length() const throw(AssertException, exception);

        /**
         * @param text the string searched.
         * @param offset the index in the string at which the search started.
         * @return the substring that matched the
         * subexpression, or an empty string if there was no match
         * for the subexpression.
         */
        string substr(const string& text, string::size_type offset = 0) const
            throw(AssertException, exception);

    private:
        bool exprMatched;
        int startIndex;
        int len;
    };
}

#endif /* _NBPP_REGEX_SUB_RESULT */
