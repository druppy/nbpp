#include <TempFile.hpp>
#include <Exception.hpp>
#include <ExceptionGuard.hpp>
#include <iostream>

using namespace std;
using namespace nbpp;

/*
 * Tests TempFile by making a temporary file, writing some text to it, and reading
 * back the text, sending it to standard out.
 */
int main(int argc, char* argv[])
{
    ExceptionGuard exceptionGuard;

    bool verbose = (argc > 1) && (strcmp(argv[1], "-v") == 0);
    
    try
    {
        TempFile f;
        if (verbose)
            cout << "Created file " << f.getPath() << endl;

        ofstream out(f.getPath().c_str());
        
        out << "First line." << endl;
        out << "Second line." << endl;

        out.close();

        ifstream in(f.getPath().c_str());
        while (in)
        {
            string line;
            getline(in, line);
            cout << line << endl;
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
