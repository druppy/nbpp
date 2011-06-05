//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco
// Rewrite to a plain parsed layout 

#include <nb++/ConfigFile.hpp>
#include <nb++/Assert.hpp>
#include <nb++/Exception.hpp>
#include <nb++/String.hpp>

#include <fstream>
#include <algorithm>

#if __GNUG__ < 3
# 	include <strstream>
#   define ostringstream ostrstream
#else
#	include <sstream>
#endif

namespace nbpp
{
    class CLex {
    public:
        enum Type {
            tEof,
            tSection,  // a section name 
            tSymbol,   // A symbol
            tData      // Data found 
        };

        CLex( istream &is ) : m_is( is ) {
            m_type = tEof;
            m_nLine = 0;
        }

        unsigned getLineNumber( void ) const { return m_nLine;}

        string curToken( void ) const {return m_sToken;}
        Type curType( void ) const {return m_type;}

        Type next( void ) {
            int ch = getch();
            m_sToken = "";

            if( ch == -1 ) 
                return m_type = tEof;

            // Remove whitespace
            while( isspace( ch )) {
                if( ch == '\n' ) {
                    m_nLine++;
                }

                if( -1 == ( ch = getch()))
                    return m_type = tEof;
            }

            // Skip plain shell comment too
            if( ch == '#' ) {
                while( -1 != ( ch = getch())) {
                    if( strchr( "\n\r", ch )) {
                        unget();
                        break;
                    }
                }
                return next();
            }

            if( ch == '[' ) { // Fetch section
                while( -1 != ( ch = getch())) {
                    if( ch != ']' )
                        m_sToken += (char)ch;
                    else 
                        break;
                }
                
                return m_type = tSection;
            }

            if( ch == '=' ) { // Ok fetch data
                while( -1 != ( ch = getch())) {
                    if( ch != '\n' && ch != '\r' ) {
                        if( ch == '\\' ) { // More than one line terminates with backslash
                            while( -1 != ( ch = getch())) {
                                if( !isspace( ch ))
                                    break;
                            }
                        }
                        m_sToken += (char)ch;
                    }
                    else 
                        break;
                }
                
                return m_type = tData;                
            }

            if( isalnum( ch ) || ch == '_' ) {  // Fetch symbol
                m_sToken += ch;
                while( -1 != ( ch = getch()))
                    if( isalnum( ch ) || ch == '_' )
                        m_sToken += (char)ch;
                    else {
                        unget();
                        break;
                    }

                return m_type = tSymbol;
            }
            return m_type = tEof;
        }
    private:
        int getch( void ) {	return m_is.get();}
        void unget( void ) {m_is.unget();}

        istream &m_is;
        Type m_type;
        string m_sToken;
        unsigned m_nLine;
    };
    
    using std::ifstream;
    using std::ofstream;
    using std::ostringstream;

    ConfigSection::ConfigSection() {}

    ConfigSection::~ConfigSection() throw() { }

    string& ConfigSection::operator[](const string& name)
    {
        iterator i;
        for( i = begin(); i != end(); i++ )
            if( i->first == name )
                break;

        if (i == end()) {
            push_back(make_pair(name, ""));
            return back().second;
        }
        
        return (*i).second;
    }

    const string& ConfigSection::operator[]( const string& name ) const
    {
        const_iterator i;
        for( i = begin(); i != end(); i++ )
            if( i->first == name )
                break;

        if (i == end())
            throw NoSuchElementException("ConfigSection::operator[] const: "
                                         + name + " element not found");
        return (*i).second;
    }

    bool ConfigSection::hasKey( const string &name ) const
    {
        for ( const_iterator i = begin(); i != end(); i++ )
            if( i->first == name )
                return true;

        return false;
    }

    bool ConfigSection::hasNonEmptyElements( void ) const 
    {
        for (const_iterator i = begin(); i != end(); i++)
            if (!(*i).second.empty())
                return true;

        return false;
    }

    ostream& ConfigSection::save( ostream& os ) const 
    {
        for (const_iterator i = begin(); i != end(); i++)
            if (!(*i).second.empty())
                os << (*i).first << " = " << (*i).second << std::endl;

        return os;
    }

    ////////////////////////////
    // Config file 

    ConfigFile::ConfigFile() {} 

    ConfigFile::ConfigFile(istream& is) 
    {
        load(is);
    }

    ConfigFile::ConfigFile(const string& file)
    {
        load(file);
    }

    ConfigFile::~ConfigFile() throw() { }

    ConfigFile &ConfigFile::operator=( const ConfigFile &cf ) {
		static_cast<multimap<string, ConfigSection>&>( *this ) = cf;
		return *this;
	}

    ConfigSection& ConfigFile::operator[](const string& name)
    {
        iterator i = find(name);
        if (i == end())
            i = insert(value_type(name, ConfigSection()));
        return (*i).second;
    }

    const ConfigSection& ConfigFile::operator[](const string& name) const
    {
        const_iterator i = find(name);
        if (i == end())
            throw NoSuchElementException("ConfigFile::operator[] const: "
                                         + name + " element not found");
        return (*i).second;
    }

    ConfigSection& ConfigFile::findFirstMatch(const string& sSection,
                                              const string& sName,
                                              const string& sValue)
    {
        std::pair<iterator,iterator> range = equal_range(sSection);
        
        for (; range.first != range.second; range.first++)
        {
            ConfigSection::iterator i;
            for( i = (*range.first).second.begin(); i != (*range.first).second.end(); i++ )
                if( i->second == sValue )
                    return (*range.first).second;
        }
        
        ConfigSection sect;
        sect[ sName ] = sValue;
        return (*insert(value_type(sSection, sect ))).second;
    }

    const ConfigSection& ConfigFile::findFirstMatch(const string& sSection,
                                                    const string& sName,
                                                    const string& sValue) const
    {
        std::pair<const_iterator,const_iterator> range = equal_range(sSection);

        for (; range.first != range.second; range.first++ ) {
            ConfigSection::const_iterator i;
            for( i = (*range.first).second.begin(); i != (*range.first).second.end(); i++ )
                if( i->second == sValue )
                    return (*range.first).second;
        }
        
        throw NoSuchElementException( "ConfigFile::findFirstMatch(): element not found" );
    }

    void ConfigFile::clear() 
    {
        erase(begin(), end());
    }

    void ConfigFile::save(const string& file) const
    {
        ofstream outputFile(file.c_str());

        if (!outputFile)
        {
            throw IOException("Can't open " + file + " for output");
        }
        else if (!save(outputFile))
        {
            throw IOException("Error saving to " + file);
        }
    }

    void ConfigFile::load(const string& file)
    {
        ifstream inputFile(file.c_str());

        if (!inputFile)
        {
            throw IOException("Can't open " + file + " for input");
        }
        else if (!load(inputFile) && !inputFile.eof())
        {
            throw IOException("Error loading " + file);
        }
    }

    ostream& ConfigFile::save(ostream& os) const
    {
        for (const_iterator i = begin(); i != end(); i++)
        {
            if ((*i).second.hasNonEmptyElements())
            {
                os << "[" << (*i).first << "]" << std::endl;
                (*i).second.save(os);
            }
        }

        return os;
    }

    void ConfigFile::throwParseException(unsigned lineNum)
    {
        ostringstream msg;
        msg << "Syntax error on line " << lineNum << " of config file";
        throw ParseException(msg.str());
    }

    // This loader uses a Config lexer and not regex's
    istream& ConfigFile::load(istream& is) 
    {
        iterator currentSection = end();
        CLex l( is );

        l.next();

        do {
            if( l.curType() == CLex::tSection )
                currentSection = insert (make_pair( trim(l.curToken()), ConfigSection()));
            else
                if( l.curType() == CLex::tSymbol ) {
                    string symbol = trim( l.curToken());

                    if( l.next() == CLex::tData ) {
                        if (currentSection != end())
                            currentSection->second.push_back(make_pair( symbol, l.curToken()));
                        else
                            throwParseException(l.getLineNumber());
                    }
                }  
                else
                    throwParseException(l.getLineNumber());
        } while( l.next() != CLex::tEof );        

        return is;
    }
}
