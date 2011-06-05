#include <CommandLineOptions.hpp>
#include <ExceptionGuard.hpp>
#include <iostream>

using namespace std;
using namespace nbpp;

int main(int argc, char* argv[])
{
    ExceptionGuard exceptionGuard;

    try
    {
        CommandLineOptions options(argc, argv, "[FILE]...");

        options.add("get foo from file", 'f', "foo", "file", CommandLineOptions::Required);
        options.add("bar, with optional port", 'b', "bar", "port", CommandLineOptions::Optional);
        options.add("output baz to file", "baz", "file", CommandLineOptions::Required);

        options.add("print verbose output", 'v', "verbose");
        options.add("produce compressed output", 'z');
        options.add("print debugging output", 'd');
        options.add("display this help and exit", 'h', "help");

        try
        {
            options.parse();
        }
        catch (ParseException& oe)
        {
            cerr << oe.getMessage() << endl;
            return 1;
        }
        catch (Exception& e)
        {
            cerr << e.toString() << endl;
            return 1;
        }
    
        if (options['h'])
        {
            options.usage();
            return 0;
        }

        OptionValue foo = options["foo"];
        if (foo)
        {
            cout << "You selected foo: " << foo.toString() << endl;
        }

        OptionValue bar = options['b'];
        if (bar)
        {
            cout << "You selected bar: " << bar.toString() << endl;
        }

        if (options["verbose"])
        {
            cout << "Verbose output selected." << endl;
        }

        if (options['d'])
        {
            cout << "Debugging output selected." << endl;
        }

        if (options['z'])
        {
            cout << "Compressed output selected." << endl;
        }

        vector<OptionValue> unmarked = options.getUnmarkedArgs();
        if (!unmarked.empty())
        {
            cout << "Files:";

            for (vector<OptionValue>::iterator iter = unmarked.begin();
                 iter != unmarked.end();
                 ++iter)
            {
                cout << " " << iter->toString();
            }

            cout << endl;
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
