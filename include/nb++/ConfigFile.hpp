//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_CONFIG_FILE
#define _NBPP_CONFIG_FILE

#include <nb++/Exception.hpp>
#include <nb++/RefCount.hpp>

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <utility>

namespace nbpp
{
    using std::string;
	using std::multimap;
    using std::istream;
    using std::ostream;
    using std::vector;
    using std::pair;

    /**
       Represents a section of a ConfigFile.  Each element in a ConfigSection represents a
       key-value pair in the corresponding section of the file.

       Data keys are stores in the same order as found in the section, of the config file.

       This class is reference-counted.  It is not thread-safe; use a Mutex to synchronize
       concurrent access.
     
       @see ConfigFile
     */
    class ConfigSection : public vector<pair<string, string> >
    {
    public:
        ConfigSection();
        ~ConfigSection() throw();

        /**
           Searches for an element having the specified key.  Returns the value of the
           first matching element found.  If not found, inserts an empty element and
           returns it.
         */
        string& operator[](const string& key);

        /**
           Searches for an element having the specified key.  Returns the value of the
           first matching element found.
         
           @exception NoSuchElementException if no matching element is found.
         */
        const string& operator[](const string& key) const;

        /**
           Return true is a key have been defined.
         */
        bool hasKey( const string &name ) const;

    private:
        bool hasNonEmptyElements( void ) const;
        ostream& save( ostream& os ) const;

        friend class ConfigFile;
    };

    /**
     * Reads, parses, modifies, and saves configuration files.  The file format used is as
     * follows:
     *
     * A config file consists of a series of sections.  Each section begins with a line
     * containing the section's name in brackets, and ends with the beginning of the next
     * section or the end of the file.  A section contains a series of key-value pairs,
     * each on a separate line.  Valid characters in section names and keys are
     * alphanumeric characters, whitespace, and '.'.
     *
     * A key and its value are separated by an '=' sign.  Whitespace before and after the
     * '=' sign is ignored.  If a value contains characters other than alphanumeric
     * characters and '.', it must be surrounded in double quotes.
     *
     * A '#' on any line begins a comment, which continues until the end of the line;
     * comments are ignored.  Blank lines are ignored.  Names of keys and sections can
     * contain spaces, but whitespace surrounding names is ignored.  There can be multiple
     * sections with the same name, and multiple keys with the same name within a section.
     * For example:
     *
     * <pre>
     * # Comment
     *
     * [foo]
     * bar = baz
     *
     * [foo2 bar]            # Comment
     * bar bar = bazbaz      # Comment
     * foo.bar = "baz baz"   # Comment
     * 
     * [foo]
     * bar = foo 
     * </pre>
     *
     * When a file is loaded, each section is represented by a ConfigSection object.
     *
     * This class is reference-counted.  It is not thread-safe; use a Mutex to synchronize
     * concurrent access.
     */
    class ConfigFile : public multimap<string, ConfigSection>
    {
    public:
        /**
         * Constructs an empty ConfigFile object.
         */
        ConfigFile();

        /**
         * Loads a config file from an input stream.
         *
         * @exception ParseException if a syntax error is found in the file.
         */
        ConfigFile(istream& is);

        /**
         * Loads a config file from the filesystem.
         *
         * @param filename the name of the file to be loaded.
         * @exception IOException if an I/O error occurs while reading the file.
         * @exception ParseException if a syntax error is found in the file.
         */
        ConfigFile(const string& filename);

        ~ConfigFile() throw();

		/**
		   Assign values from another ConfigFile object.
		 */
		ConfigFile &operator=( const ConfigFile &cf );

        /**
         * Returns the first ConfigSection found that has the specified name.
         * Inserts an empty element if not found.
         */
        ConfigSection& operator[](const string& name);

        /**
         * Returns the first ConfigSection found that has the specified name.
         *
         * @exception NoSuchElementException if no matching section is found.
         */
        const ConfigSection& operator[](const string& name) const;

        /**
         * Clears the contents of this ConfigFile object.
         */
        void clear();

        /**
         * Returns the first ConfigSection found that has the specified name, and contains
         * an element with the specified name and value.  If no such section is found, it
         * is created and returned.
         */
        ConfigSection& findFirstMatch(const string& section,
                                      const string& name,
                                      const string& value);

        /**
         * Returns the first ConfigSection found that has the specified name, and contains
         * an element with the specified name and value.
         *
         * @exception NoSuchElementException if no matching section is found.
         */
        const ConfigSection& findFirstMatch(const string& section,
                                            const string& name,
                                            const string& value) const;
        
        /**
         * Saves the config file.
         *
         * @param filename the name of the file to be saved.
         * @exception IOException if an I/O error occurs while saving the file.
         */
        void save(const string& filename) const;

        /**
         * Loads a config file from the filesystem.
         *
         * @param filename the name of the file to be loaded.
         * @exception IOException if an I/O error occurs while loading the file.
         * @exception ParseException if a syntax error is found in the file.
         */
        void load(const string& filename);
        
        /**
         * Saves the config file to an output stream.
         */
        ostream& save(ostream& os) const;

        /**
         * Reads a config file from an input stream.
         *
         * @exception ParseException if a syntax error is found in the file.
         */
        istream& load(istream& is);

    private:
        void throwParseException(unsigned lineNum);
    };
}

#endif /* _NBPP_CONFIG_FILE */
