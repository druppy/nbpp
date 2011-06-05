/**
   Contain some nb++ string helper definitions and functions.
*/
#ifndef __STRING_HPP__
#define __STRING_HPP__

#include <string>
#include <vector>

#include <nb++/osdep.hpp>

namespace nbpp {
	using namespace std;

	typedef vector<string> Strings;

	/**
	   Trim the string both left and right for whitespaces.

	   @param str the string to trim
	   @return the newly trimed string 
	 */
	string trim( const string &str );

	/**
	   Trim all whitespace on the left of the string.

	   @param str the string to trim
	   @return the newly trimed string 
	 */
	string ltrim( const string &str );

	/**
	   Trim all whitespace on the right of the string.

	   @param str the string to trim
	   @return the newly trimed string 
	 */
	string rtrim( const string &str );

	/**
	   Splits a string into substrings, using the specified character as a delimiter.
     
	   @param text the string to be split.
	   @param delim the character to be used as a delimiter.
	   @return a Strings containing the substrings.
     */
    Strings split( const string& text, char delim );

    /**
	   Splits a string into substrings, using the specified string as a delimiter.
     
	   @param text the string to be split.
	   @param delim the string to be used as a delimiter.
	   @return a Strings containing the substrings.
     */
    Strings split( const string& text, const string& delim );
}

#endif
