//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/CommandLineOptions.hpp>

#include <list>
#include <utility>
#include <algorithm>
#if __GNUG__ < 3
#	include <strstream>
#   define ostringstream ostrstream
#else 
#	define ends ""
#	include <sstream>
#endif 
#include <cstdlib>
#include <cctype>
#include <string>

extern "C"
{
#include <getopt.h>

#ifdef NBPP_HAVE_GETOPT_H
#  ifdef NBPP_HAVE_GETOPT_LONG
#    include <getopt.h>
#  else
#    include "os/getopt_long.h"
#  endif
#else
#  include "os/getopt_long.h"
#endif
}

namespace nbpp
{
	using namespace std;

    OptionValue::OptionValue() : present(false) { }

    OptionValue::OptionValue(const string& arg_text) : present(true), text(arg_text) { }

    const string& OptionValue::toString() const 
    {
        return text;
    }

    OptionValue::operator bool() const throw()
    {
        return present;
    }

    int OptionValue::toInt() const 
    {
        return atoi(text.c_str());
    }

    CommandLineOptions::Option::Option() :
        present(false), shortName(0), optionArg(NoArgument) { }

    CommandLineOptions::Option::Option(const string& arg_description,
                                       char arg_shortName,
                                       const string& arg_longName,
                                       OptionArgument arg_optionArg,
                                       const string& arg_argType) :
        present(true), description(arg_description), shortName(arg_shortName),
        longName(arg_longName), optionArg(arg_optionArg), argType(arg_argType)
    { }

    CommandLineOptions::CommandLineOptions(int arg_argc, char** arg_argv,
                                           const string& arg_unmarkedTypes) :
        argc(arg_argc), argv(arg_argv), unmarkedTypes(arg_unmarkedTypes) { }
        
    void CommandLineOptions::add(const string& description,
                                 char shortName)
    {
        add(description, shortName, "", "", NoArgument);
    }
    
    void CommandLineOptions::add(const string& description,
                                 char shortName,
                                 const string& argType,
                                 OptionArgument optionArg)
    {
        add(description, shortName, "", argType, optionArg);
    }
    
    void CommandLineOptions::add(const string& description,
                                 const string& longName)
    {
        add(description, 0, longName, "", NoArgument);
    }
    
    void CommandLineOptions::add(const string& description,
                                 const string& longName,
                                 const string& argType,
                                 OptionArgument optionArg)
    {
        add(description, 0, longName, argType, optionArg);
    }
    
    void CommandLineOptions::add(const string& description,
                                 char shortName,
                                 const string& longName)
    {
        add(description, shortName, longName, "", NoArgument);
    }
        
    void CommandLineOptions::add(const string& description,
                                 char shortName,
                                 const string& longName,
                                 const string& argType,
                                 OptionArgument optionArg)
    {
        Option opt(description, shortName, longName, optionArg, argType);

        if (shortName)
        {
            shortOptions[shortName] = opt;
        }

        if (!longName.empty())
        {
            longOptions.push_back(opt);
        }
    }

    void CommandLineOptions::parse( bool bIgnoreUnknown ) 
    {
        opterr = 0;

        shortOptionValues.clear();
        longOptionValues.clear();
        unmarkedArgs.clear();
        
        // Make a string of short-option characters for getopt_long().
        string shortOptionChars = ":";
        for (ShortOptionMap::iterator shortIter = shortOptions.begin();
             shortIter != shortOptions.end();
             ++shortIter)
        {
            Option opt = shortIter->second;
            shortOptionChars += opt.shortName;

            switch(opt.optionArg)
            {
            case Optional:
                shortOptionChars += ':';

            case Required:      // fall-thru
                shortOptionChars += ':';

            case NoArgument:    // fall-thru
                break;
            }
        }

        // Make an array of struct option objects, representing long options,
        // for getopt_long().
        option longGNUOptions[longOptions.size() + 1];
        int i = 0;
        for (LongOptionVec::iterator longIter = longOptions.begin();
             longIter != longOptions.end();
             ++longIter)
        {
            longGNUOptions[i].name = longIter->longName.c_str();

            switch(longIter->optionArg)
            {
            case NoArgument:
                longGNUOptions[i].has_arg = no_argument;
                break;

            case Optional:
                longGNUOptions[i].has_arg = optional_argument;
                break;

            case Required:
                longGNUOptions[i].has_arg = required_argument;
                break;
            }

            longGNUOptions[i].flag = 0;
            
            // Set the high bit of the value of long options, so we can distinguish
            // between long and short options returned by getopt_long().
            longGNUOptions[i].val = 256 + i;
            ++i;
        }

        longGNUOptions[i].name = 0;
        longGNUOptions[i].has_arg = 0;
        longGNUOptions[i].flag = 0;
        longGNUOptions[i].val = 0;

        // Get options and their arguments from the command line.
        while (true)
        {
            int optionIndex = 0;
            int c = getopt_long(argc, argv, shortOptionChars.c_str(),
                                longGNUOptions, &optionIndex);

            // No more options.
            if (c == -1) break;

            // An error was found.
            if ( c == ':' )
            {
                throw ParseException(getoptErrorStr(c == ':'));
            }

			if( c == '?' ) {
				if( bIgnoreUnknown )
					break; // Go to the next option
				else
					throw ParseException(getoptErrorStr(c == ':'));	
			}

            // Got some kind of option; get its argument, if any.
            string text;
            if (optarg)
            {
                text = optarg;
            }
            OptionValue value(text);

            if (c >= 256)
            {
                // Got a long option.
                Option opt = longOptions[optionIndex];
                longOptionValues[opt.longName] = value;

                if (opt.shortName)
                {
                    shortOptionValues[opt.shortName] = value;
                }
            }
            else
            {
                // Got a short option.
                Option opt = shortOptions[c];
                shortOptionValues[c] = value;
                
                if (!opt.longName.empty())
                {
                    longOptionValues[opt.longName] = value;
                }
            }
        }

        // Get any non-option arguments.
        for (i = optind; i < argc; ++i)
        {
            unmarkedArgs.push_back(OptionValue(argv[i]));
        }
    }

    const int CommandLineOptions::COLUMN_SPACING = 4;

    void CommandLineOptions::usage(ostream& errstr)
    {
        Lock lock(mutex);
        // We'll collect lines in a list<> before printing, to determine the width
        // of the first column.
        typedef pair<string, string> Line;
        list<Line> lines;
        string::size_type colWidth = 0;

        string usedChars; // short forms that also have long forms

        // Add the long forms, with corresponding short forms.
        for (LongOptionVec::iterator longIter = longOptions.begin();
             longIter != longOptions.end();
             ++longIter)
        {
            Option opt = *longIter;
            string col = "--";
            col += opt.longName;
            addArgType(col, opt, true);

            if (opt.shortName)
            {
                col.append(", -") += opt.shortName;
                usedChars += opt.shortName;
            }

            addArgType(col, opt, false);
            if (col.size() > colWidth) colWidth = col.size();
            lines.push_back(Line(col, opt.description));
        }

        // Add any remaining short forms.
        for (ShortOptionMap::iterator shortIter = shortOptions.begin();
             shortIter != shortOptions.end();
             ++shortIter)
        {
            Option opt = shortIter->second;

            if (usedChars.find(opt.shortName) == string::npos)
            {
                string col = "-";
                col += opt.shortName;
                addArgType(col, opt, false);
                if (col.size() > colWidth) colWidth = col.size();
                lines.push_back(Line(col, opt.description));
            }
        }

        // Print the usage summary, padding the first column to the appropriate
        // width.
        colWidth += COLUMN_SPACING;
        errstr << "Usage: " << argv[0] << " [OPTION]... " << unmarkedTypes << endl << endl;
        for (list<Line>::iterator lineIter = lines.begin();
             lineIter != lines.end();
             ++lineIter)
        {
            errstr << pad(lineIter->first, colWidth) << lineIter->second << endl;
        }
    }

    // This appears to be necessary because setw() doesn't seem to work.
    string& CommandLineOptions::pad(string& col, string::size_type width)
    {
        if (col.size() < width)
        {
            col += string(width - col.size(), ' ');
        }
        return col;
    }

    // Adds an Option's argument type string to a line of usage output, surrounded
    // by brackets if it's an optional argument.
    string& CommandLineOptions::addArgType(string& col,
                                           const Option& opt,
                                           bool longOption)
    {
        const string argType = !opt.argType.empty() ? opt.argType : "arg";

        switch(opt.optionArg)
        {
        case NoArgument:
            break;
        case Optional:
            if (longOption)
            {
                col.append("[=").append(opt.argType).append("]");
            }
            else if (opt.shortName)
            {
                col.append("[").append(opt.argType).append("]");
            }
            break;
        case Required:
            // Don't add a required argument after a long form, because we'll be called
            // again to add it after the short form (if any) has been added.
            if (!longOption)
            {
                col.append(" ").append(argType);
            }
            break;
        }

        return col;
    }

    CommandLineOptions::LongNameStartsWith::LongNameStartsWith(const string& arg_substr)
        : substr(arg_substr) { }

    bool CommandLineOptions::LongNameStartsWith::operator()(const
                                                            CommandLineOptions::Option&
                                                            option) const
    {
        return option.longName.substr(0, substr.length()) == substr;
    }

    string CommandLineOptions::getoptErrorStr(bool missingArg) const
    {
        ostringstream message;

        if (optopt == 0)
        {
            string optionStr = argv[optind - 1];
            if (find_if(longOptions.begin(), longOptions.end(),
                        LongNameStartsWith(optionStr.substr(2))) != longOptions.end())
            {
                message << "option `" << optionStr << "' is ambiguous" << ends;
            }
            else
            {
                message << "unrecognized option `" << optionStr << "'" << ends;
            }
        }
        else if (optopt >= 256)
        {
            if (missingArg)
            {
                message << "option `--" << longOptions[optopt - 256].longName
                        << "' requires an argument" << ends;
            }
            else
            {
                message << "option `--" << longOptions[optopt - 256].longName
                        << "' doesn't allow an argument" << ends;
            }
        }
        else
        {
            if (missingArg)
            {
                message << "option requires an argument -- " << (char)optopt << ends;
            }
            else
            {
                message << "invalid option -- " << (char)optopt << ends;
            }
        }

        return message.str();
    }

    const OptionValue& CommandLineOptions::operator[]( char shortName )
    {
        return shortOptionValues[shortName];
    }

	bool CommandLineOptions::hasA( char shortName ) const
	{
		return shortOptionValues.count( shortName ) > 1; 
	}

    const OptionValue& CommandLineOptions::operator[]( const string& longName )        
    {
        return longOptionValues[ longName ];
    }

	bool CommandLineOptions::hasA( const string &sLongName ) const
	{
		return longOptionValues.count( sLongName ) > 1;
	}

    const OptionValues &CommandLineOptions::getUnmarkedArgs() const 
    {
        return unmarkedArgs;
    }
}
