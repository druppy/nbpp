#include <iostream>
#include <Regex.hpp>
#include <ExceptionGuard.hpp>

/*
 * Demonstrates Regex.  The user enters a line of text, and the
 * program looks for an email address somewhere in the text, using a
 * vaguely email-like (but hopelessly non-RFC-822-compliant) pattern.
 * Matched substrings are displayed.
 */

using namespace std;
using namespace nbpp;

int main(int argc, char* argv[])
{
    ExceptionGuard exceptionGuard;

    try
    {
        string pattern =
            "([[:alnum:]]+)(\\.[[:alnum:]]+)*@([[:alnum:]]+\\.)+([[:alnum:]]{2,3})";

        Regex addrRegex(pattern);
        string address;

        cout << "Pattern: " << pattern << endl;
        cout << "Enter text containing an email address, or \"q\" to quit." << endl;
            
        while (true)
        {
            cout << "? ";
            getline(cin, address);

            if (address == "q")
            {
                break;
            }

            RegexResult result = addrRegex.match(address);

            if (result.matched())
            {
                cout << "Match: " <<
                    address.substr(result.start(),
                                   result.length()) << endl;

                // Display substrings that matched subexpressions.
                for (int i = 1; i <= addrRegex.getSubExprCount(); i++)
                {
                    RegexSubResult subResult = result[i];
                    if (subResult.matched())
                    {
                        cout << "Subexpression " << i << ": " <<
                            address.substr(subResult.start(),
                                           subResult.length()) << endl;
                    }
                    else
                    {
                        cout << "No match for subexpression " << i <<
                            "." << endl;
                    }
                }

		cout << "And the simple way " << endl;

		Strings strs = result.getGroups( address );
		Strings::const_iterator i;
		for( i = strs.begin(); i != strs.end(); i++ )
			cout << "Result : " << *i << endl;
            }
            else
            {
                cout << "Match not found." << endl;
            }
        }
    }
    catch (Exception& e)
    {
        cerr << e.toString() << endl;
        return 1;
    }
    catch (exception& e)
    {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
