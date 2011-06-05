#include <nb++/String.hpp>

#include <cctype>

namespace nbpp {
	using namespace std;

	string trim( const string &str ) 
	{
		return rtrim( ltrim( str ));
	}

	string rtrim( const string &str )
	{
		string::const_reverse_iterator iter = str.rbegin();

		while( iter != str.rend() && isspace( *iter ))
			iter++;

		return str.substr( 0, str.length() - (iter - str.rbegin()) );
	}

	string ltrim( const string &str )
	{
		string::const_iterator iter = str.begin();
		while( iter != str.end() && isspace( *iter ))
			iter++;

		return string( iter, str.end());
	}

    Strings split( const string& text, char sep)
    {
        Strings vec;

        string::size_type pos = 0;
        for (string::size_type newp; (newp = text.find(sep, pos)) != string::npos;
             pos = newp + 1)
            vec.push_back(text.substr(pos, newp - pos));
        vec.push_back(text.substr(pos));

        return vec;
    }

    Strings split(const string& text, const string& sep)
    {
        Strings vec;

        string::size_type pos = 0;
        for (string::size_type newp; (newp = text.find(sep, pos)) != string::npos;
             pos = newp + sep.length())
            vec.push_back(text.substr(pos, newp - pos));
        vec.push_back(text.substr(pos));

        return vec;
    }

};
