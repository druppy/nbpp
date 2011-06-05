#include <CommandLineOptions.hpp>
#include <ExceptionGuard.hpp>
#include <fstream>
#if __GNUG__ < 3
#	include <strstream>
#   define ostringstream ostrstream
#else 
#	include <sstream>
#endif 
#include <iomanip>

/*
 * Counts the number of lines of source code in one or more files, not including
 * blank lines or lines that consist only of whitespace or comments.
 */

using namespace std;
using namespace nbpp;

const int COL_WIDTH = 7; // width of the column of numbers in the output

int countLines(const string& filename) throw(IOException, AssertException, exception)
{
    ifstream is(filename.c_str());
    if (!is)
    {
        ostringstream msg;
        msg << "Can't read from file: " << filename;
        throw IOException(msg.str());
    }

    int lineCount = 0;
    bool inComment = false;
    string line;
    while (is)
    {
        getline(is, line);
        string::size_type pos = 0;

        // Look for code on the current line.
        while (true)
        {
            // If we're at the end of this line, skip it.
            if (pos >= line.length()) break;

            // Find the first non-whitespace character.
            pos = line.find_first_not_of(" \t", pos);

            // If there is none, skip the line.
            if (pos == string::npos) break;
            
            if (inComment)
            {
                // If we're in a C-style comment, see if it ends on this line.
                string::size_type commentEnd = line.find("*/", pos);
                if (commentEnd == string::npos)
                {
                    // If not, skip the line.
                    break;
                }
                else
                {
                    // If so, look for code on the rest of the line.
                    pos = commentEnd + 2;
                    inComment = false;
                    continue;
                }
            }
            else
            {
                // We're not in a C-style comment.  See if the next thing
                // is a comment of some sort.
                if (line[pos++] == '/')
                {
                    if (pos < line.length())
                    {
                        char c = line[pos++];
                        if (c == '*')
                        {
                            // The next thing is a C-style comment.  See if it ends on
                            // this line.
                            inComment = true;
                            continue;
                        }
                        else if (c == '/')
                        {
                            // The next thing is a C++-style comment.  Skip the
                            // line.
                            break;
                        }
                    }
                }

                // We found some code on this line, so count it.
                ++lineCount;
                break;
            }
        }
    }

    return lineCount;
}

int main(int argc, char* argv[])
{
    ExceptionGuard exceptionGuard;

    try
    {
        CommandLineOptions options(argc, argv, "[FILE]...");

        options.add("show counts for individual files", 'a', "all");
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
    
        vector<OptionValue> unmarked = options.getUnmarkedArgs();

        if (options["help"] || unmarked.empty())
        {
            options.usage();
            return 0;
        }

        int totalLines = 0;
        for (vector<OptionValue>::iterator iter = unmarked.begin();
             iter != unmarked.end();
             ++iter)
        {
            string filename = iter->toString();
            int fileLines = countLines(filename);
            if (options["all"])
            {
                cout << setw(COL_WIDTH) << fileLines << " " << filename << endl;
            }
            totalLines += fileLines;
        }

        cout << setw(COL_WIDTH) << totalLines << " total line(s) of source code" << endl;
    }
    catch (const Exception& e)
    {
        cerr << e.toString() << endl;
        return 1;
    }
    catch (const exception& e)
    {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
