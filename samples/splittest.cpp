#include <String.hpp>
#include <Regex.hpp>
#include <ExceptionGuard.hpp>
#include <iostream>
#include <iterator>
#include <string>

using namespace std;
using namespace nbpp;

/*
 * Tests split().
 */

int main(int argc, char* argv[])
{
    ExceptionGuard exceptionGuard;
	try
    {
		string s( "    abc    " );

		cout << '\"' << s << '\"' << endl;
		cout << '\"' << ltrim( s ) << '\"' << endl;
		cout << '\"' << rtrim( s ) << '\"' << endl;
		cout << '\"' << trim( s ) << '\"' << endl;
    
        Regex re("[:;]+");
        ostream_iterator<string> scout(cout, ", ");

        vector<string> vec = split("a:b::c;d:e", ':');
        cout << vec.size() << ": ";
        copy (vec.begin(), vec.end(), scout);
        cout << endl;
        // Output: 5: a, b, , c;d, e,

        vec = split("a:b::c;d:e", "::");
        cout << vec.size() << ": ";
        copy (vec.begin(), vec.end(), scout);
        cout << endl;
        // Output: 2: a:b, c;d:e,

        vec = split("a:b::c;d:e", re);
        cout << vec.size() << ": ";
        copy (vec.begin(), vec.end(), scout);
        cout << endl;
        // Output: 5: a, b, c, d, e,
    }
    catch ( const Exception& e )
    {
        cerr << e.toString() << endl;
        return 1;
    }
    catch ( const exception& e )
    {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
