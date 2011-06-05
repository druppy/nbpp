//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_COMMAND_LINE_OPTIONS
#define _NBPP_COMMAND_LINE_OPTIONS

#include <nb++/Exception.hpp>
#include <nb++/Mutex.hpp>
#include <nb++/RefCount.hpp>

#include <map>
#include <vector>
#include <string>
#include <iostream>

namespace nbpp
{
    using std::vector;
    using std::map;
    using std::string;
    using std::ostream;
    using std::cerr;

    /**
     * Represents an option entered on the command line.
     *
     * This class is not reference-counted, but is inexpensive to pass
     * by value.
     *
     * @see CommandLineOptions
     */
    class OptionValue //: public RefCounted
    {
    public:
        OptionValue();

        explicit OptionValue(const string& text);

        /**
         * @return the argument provided with this option, if any.
         */
        const string& toString() const;

        /**
         * @return true if this option was entered.
         */
        operator bool() const throw();

        /**
         * @return the integer value of the argument provided with this option, if any.
         */
        int toInt() const;

    private:
        bool present;
        string text;
    };

	// typedef vector<RefHandle<OptionValue> > OptionValues;
	typedef vector<OptionValue> OptionValues;

    /**
     * Parses command-line options.  Each option may have a short form, a long form, or
     * both.  To use this class, call the add() methods to describe the options your
     * program accepts, then call parse().  You can then use the operator[] methods to get
     * OptionValue objects representing the options that were found on the command line.
     * The usage() method prints a standard usage summary.
     *
     * This class is not reference-counted.  Since it uses static variables provided by
     * GNU getopt_long(), there should be only one CommandLineOptions object in a program.
     * Once options have been parsed, multiple threads can call the operator[] and
     * getUnmarkedArgs() methods; the other methods are not thread-safe.
     *
     * @see OptionValue
     */
    class CommandLineOptions
    {
    public:
        /**
         * Constants for the add() methods.
         */
        enum OptionArgument
        {
            NoArgument,  /**< The option doesn't accept an argument. */
            Optional,    /**< The option can take an argument. */
            Required     /**< The option requires an argument. */
        };

        /**
         * Constructs a CommandLineOptions object using the arguments passed to main().
         *
         * @param argc the argc passed to main().
         * @param argv the argv passed to main().
         * @param unmarkedTypes a string listing the types of any expected non-option
         * arguments, e.g. "[FILE]..."; this will added to the first line of the
         * usage summary printed by usage().
         */
        CommandLineOptions(int argc, char** argv,
                           const string& unmarkedTypes = "");

        /**
         * Adds an option to the list of options accepted by the program.  No argument is
         * expected for the option.
         *
         * @param description a description of the option.
         * @param shortName the short form of the option.
         */
        void add(const string& description, char shortName);

        /**
         * Adds an option to the list of options accepted by the program.
         *  
         * @param description a description of the option.
         * @param shortName the short form of the option.
         * @param argType a string describing the type of argument expected.
         * @param optionArg option's argument is NoArgument, Optional or Required.
         */
        void add(const string& description, char shortName, const string& argType,
                 OptionArgument optionArg);

        /**
         * Adds an option to the list of options accepted by the program.  No argument is
         * expected for the option.
         *  
         * @param description a description of the option.
         * @param longName the short form of the option.
         */
        void add(const string& description, const string& longName);

        /**
         * Adds an option to the list of options accepted by the program.
         *  
         * @param description a description of the option.
         * @param longName the long form of the option.
         * @param argType a string describing the type of argument expected.
         * @param optionArg option's argument is NoArgument, Optional or Required.
         */
        void add(const string& description, const string& longName,
                 const string& argType, OptionArgument optionArg);

        /**
         * Adds an option to the list of options accepted by the program.  No argument is
         * expected for the option.
         *  
         * @param description a description of the option.
         * @param shortName the short form of the option.
         * @param longName the long form of the option.
         */
        void add(const string& description, char shortName, const string& longName);

        
        /**
         * Adds an option to the list of options accepted by the program.
         *  
         * @param description a description of the option.
         * @param shortName the short form of the option.
         * @param longName the long form of the option.
         * @param argType a string describing the type of argument expected.
         * @param optionArg option's argument is NoArgument, Optional or Required.
         */
        void add(const string& description, char shortName, const string& longName,
                 const string& argType, OptionArgument optionArg);

        /**
		   Parses the command-line options.
         
		   @param bIgnoreUnknown true if unknown parameters has to be ignored (default:false)

		   @exception ParseException if an error is found.  The exception's detail message
		   is suitable for printing as a response to the user.
         */
        void parse( bool bIgnoreUnknown = false );

        /**
         * Prints a usage summary.  The first line is "Usage: " followed by the
         * name of the program (from argv[0]), "[OPTIONS]...", and the contents
         * of the string "unmarkedTypes" passed to the CommandLineOptions constructor.
         *
         * This is followed by a blank line, then a list of all supported options, in two
         * columns.  The first column contains the name(s) (and argument type, if any), of
         * each option; the second column contains the option's description.  Options that
         * have long forms are listed first, in the order in which they were added with
         * add(); short forms are listed with the corresponding long forms.  Then any
         * short forms that don't have long forms are listed, in alphabetical order.
         * The types of optional arguments are given in brackets.  For example:
         *
         * <pre>
         * Usage: optiontest [OPTION]... [FILE]...
         *
         * --foo, -f file            get foo from file
         * --bar[=port], -b[port]    bar, with optional port
         * --baz file                output baz to file
         * --verbose, -v             print verbose output
         * --help, -h                display this help and exit
         * -d                        print debugging output
         * -z                        produce compressed output
         * </pre>
         *
         * @param errstr the output stream to which the usage summary should be printed.
         */
        void usage(ostream& errstr = cerr);

        /**
         * @return an OptionValue representing an expected option, identified by its
         * short form.  The OptionValue will evaluate to false if it was not found.
         *
         * @param shortName the short name of the option.
         */
        const OptionValue& operator[](char shortName);

		/**
		   Check to see if the short option name is present.

		   @param shortName the name of the short option to test for 
		   @return true if option is present		   
		 */
		bool hasA( char shortName ) const;

        /**
         * @return an OptionValue representing an expected option, identified by its
         * long form.  The OptionValue will evaluate to false if it was not found.
         *
         * @param longName the long name of the option.
         */
        const OptionValue& operator[](const string& longName );

		/**
		   Check to see if the long option name is present.

		   @param sLongName the name of the long option to test for 
		   @return true if option is present
		 */
		bool hasA( const string &sLongName ) const;

        /**
         * @return a Vector<OptionValue> containing any non-option arguments found on the
         * command line.
         */
        const OptionValues &getUnmarkedArgs() const;

    private:
        CommandLineOptions(const CommandLineOptions&);
        CommandLineOptions& operator=(const CommandLineOptions&);

        struct Option
        {
            Option();
            Option(const string& description, char shortName,
                   const string& longName, OptionArgument optionArg,
                   const string& argType);
            bool present;
            string description;
            char shortName;
            string longName;
            OptionArgument optionArg;
            string argType;
        };
        
        // A predicate that returns true if an option's long name begins with the
        // specified substring.
        class LongNameStartsWith
        {
        public:
            LongNameStartsWith(const string& substr);
            
            bool operator()(const CommandLineOptions::Option& option) const;

        private:
            string substr;
        };

        int argc;
        char** argv;
        string unmarkedTypes;
        Mutex mutex;
        static const int COLUMN_SPACING;

        typedef map<char, Option> ShortOptionMap;
        ShortOptionMap shortOptions;
        
        typedef vector<Option> LongOptionVec;
        LongOptionVec longOptions;

        typedef map<char, OptionValue> ShortOptionValueMap;
        ShortOptionValueMap shortOptionValues;

        typedef map<string, OptionValue> LongOptionValueMap;
        LongOptionValueMap longOptionValues;

        OptionValues unmarkedArgs;

        string& addArgType(string& col, const Option& opt, bool longOption);
        string& pad(string& col, string::size_type width);

        string getoptErrorStr(bool missingArg) const;
    };
}

#endif /* _NBPP_COMMAND_LINE_OPTIONS */
