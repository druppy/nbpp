#include <nb++/Log.hpp>
#include <nb++/Time.hpp>

#include <cstdarg>
#include <cstdio>

#include <string>
#include <sstream>
#include <stdexcept>

namespace nbpp {

///////////////////////////////
// Impl. of ErrStreamLogger

class ErrStreamLogger : public Logger
{
public:
	void put( int nCode, const string &sParam ); 
	void put( const string &sLine );
};

void ErrStreamLogger::put( int nCode, const string &sParam )
{
	Time t;

	cerr << t.toString() << ';' << nCode << ';' << sParam << endl;
}

void ErrStreamLogger::put( const string &sLine )
{
	Time t;

	cerr << t.toString() << ";0;" << sLine << endl;
}

//////////////////////////////////
// Impl. of FileLogger

FileLogger::FileLogger( const string &sFName, bool bParanoid )
{
	m_bParanoid = bParanoid;
	m_sFName = sFName;
	m_nLineCount = 0;

	m_os.open( m_sFName.c_str(), ios::out|ios::app );
}

void FileLogger::put( int nCode, const string &sParam )
{
	Lock sync( m_mutex );
	Time t;

	m_os << t.toString() << ';' << nCode << ";" << sParam << endl;

	if( m_bParanoid ) {
		if( m_nLineCount % 16 == 0 ) {
			m_os.close();
			m_os.open( m_sFName.c_str(), ios::out|ios::app );
		}
		else
			m_os.flush();
	}
}

void FileLogger::put( const string &sLine )
{
	Lock sync( m_mutex );
	
	Time t;
	
	m_os << t.toString() << ";0;" << sLine << endl;

	if( m_bParanoid ) {
		if( m_nLineCount % 16 == 0 ) {
			m_os.close();
			m_os.open( m_sFName.c_str(), ios::out|ios::app );
		}
		else
			m_os.flush();
	}
}

/////////////////////////////////
// Def. and impl. of TinyLexer

class TinyLexer {
public:
	enum Type {
		tEos,         // End of string
		tSymbol,      // symbol 
		tString,      // "str"
		tNumber,      // a number
		tDelim,       // ,;\t
		tError
	};

	TinyLexer( const string &str );

	Type nextToken( void );

	string getToken( void ) {return m_sToken;}
	Type getType( void ) {return m_type;}
private:
	string m_str;
	string::iterator m_cur;
	string m_sToken;
	Type m_type;
};

TinyLexer::TinyLexer( const string &str )
{
	m_str = str;
	m_cur = m_str.begin();
}

TinyLexer::Type TinyLexer::nextToken( void )
{
	while( isspace( *m_cur ) && m_cur != m_str.end())
		m_cur++;

	if( m_cur == m_str.end())
		return m_type = tEos;

	if( strchr( ":;\t,", *m_cur ) ) {
		m_sToken = *m_cur++;
		return m_type = tDelim;
	}

	if( *m_cur == '\"' ) {
		string str;
		while( ++m_cur != m_str.end() && *m_cur != '\"' ) {
			if( *m_cur == '\\' )
				str += *m_cur++;
			 
			str += *m_cur;
		}

		if( m_cur != m_str.end() )
			m_cur++;

		m_sToken = str;
		return m_type = tString;
	}
	
	if( isalpha( *m_cur ) || *m_cur == '_' ) {
		string str;
		do {
			str += *m_cur;
		} while( ++m_cur != m_str.end() && (isalnum( *m_cur ) || *m_cur == '_' ));

		m_sToken = str;
		return m_type = tSymbol;
	}

	if( isdigit( *m_cur ) ) {
		string str;
		do {
			str += *m_cur;
		} while( ++m_cur != m_str.end() && (isdigit( *m_cur ) || *m_cur == '.' ));
		
		m_sToken = str;
		return m_type = tNumber;
	}

	return m_type = tError;
}

//////////////////////////
// Impl. of LogLine 

static bool fetch_param( istringstream &is, string &sName, string &sValue )
{
	if( getline( is, sName, '=' ))
		if( getline( is, sValue, ';' ))
			return true;

	return false;
}

LogLine::LogLine( const LogDefinition &def, const string &sLine )
{
	string sTimestamp;
	istringstream is( sLine );
	m_nCode = 0;
	int ch;
	
	getline( is, sTimestamp, ';' );

	if( !m_timestamp.setString( sTimestamp ))
		cerr << "Error in timestamp" << endl;

	// Check if it is a normal logline of do we have a code
	is >> m_nCode;
	if( -1 == (ch = is.get()) || ch != ';' )
		return;

	if( m_nCode && ch == ';' ) {
		map<string,string> params;
		string sName, sText;

		// Parse the parameters
		while( fetch_param( is, sName, sText )) {
			// cout << "Param " << sName << " = " << sText << endl; 
			params[ sName ] = sText;
		}

		// The merge line from the def. table with out parameters
		m_nLevel = def.getLevel( m_nCode );
		string sResult, sRaw = def.getRawText( m_nCode );
		for( string::iterator i = sRaw.begin(); i != sRaw.end(); i++ ) {
			if( *i == '\\' )
				i++;

			if( *i == '%' ) {
				string sParam = "";
				i++;

				for( ;(isalnum( *i ) || *i == '_') && i != sRaw.end(); i++ )
					sParam += *i;

				i--;
				if( params.count( sParam ) == 1 )
					sResult += params[ sParam ];
				else
					sResult += '?';
			}
			else
				sResult += *i;
		}
		m_sText = sResult;
	}
	else {
		getline( is, m_sText );
	}
}

///////////////////////////
// Impl. of LogDefinition
	
LogDefinition::LogDefinition( const string &sDefFile )
{
	loadDefs( sDefFile );
}

int LogDefinition::loadDefs( const string &sDefFile )
{
	int nLine = 0, nCode = 100;
	string sLine;
	ifstream is( sDefFile.c_str());

	while( getline( is, sLine )) {
		nLine++;
		LineDef def;
		string sName, sType;
		TinyLexer lex( sLine );

		if( TinyLexer::tSymbol == lex.nextToken()) {
			sName = lex.getToken();
			if( TinyLexer::tDelim == lex.nextToken()) {
				if( TinyLexer::tSymbol == lex.nextToken()) {
					sType = lex.getToken();
					if( TinyLexer::tDelim == lex.nextToken()) {
						if( TinyLexer::tString == lex.nextToken()) {
							def.sLine = lex.getToken();

							if( sType == "INFO" || sType == "INF" )
								def.l = Info;
							if( sType == "WARNING" || sType == "WRN" )
								def.l = Warning;
							if( sType == "ERROR" || sType == "ERR" )
								def.l = Error;
							if( sType == "FATAL" || sType == "FTL")
								def.l = Fatal;

							// cout << "name " << sName << " = " << def.sLine << endl;
							if( m_codes.count( nCode ) ) {
								cout << sName << " at line " << nLine;
								cout << " already exist at "<< nCode << " , owerwriting !" << endl;
							}

							m_codes[ nCode++ ] = def;
							// cout << "Code " << nCode << " = " << sName << endl;
							continue;
						}
					}
				}
			}
		}

		if( lex.getToken() == ":" )
			if( lex.nextToken() == TinyLexer::tNumber ) 
				if( 1 == sscanf( lex.getToken().c_str(), "%d", &nCode ))
					continue;
		
		if( lex.getType() == TinyLexer::tEos )
			continue;

		cerr << "Syntax error in line " << nLine << " of " << sDefFile << endl; 
	}
	return nLine;
}

LogDefinition::Level LogDefinition::getLevel( int nCode ) const
{
	if( m_codes.count( nCode ) ) {
		codes_t::const_iterator i = m_codes.find( nCode );

		if( i != m_codes.end())
			return i->second.l;
	}

	throw invalid_argument( "Unknown code" );
}

string LogDefinition::getRawText( int nCode ) const
{
	if( m_codes.count( nCode ) ) {
		codes_t::const_iterator i = m_codes.find( nCode );

		if( i != m_codes.end())
			return i->second.sLine;
	}

	throw invalid_argument( "Unknown code" );
}


LogLine LogDefinition::parseLine( const string &sLine ) const
{
	return LogLine( *this, sLine );
}

///////////////////////////
// Impl. of Log 

class LogStreamBuf : public streambuf
{
	int m_nCode;
	string m_sLine;
	Log &m_log;
	Mutex m_mutex;
public:
	LogStreamBuf( Log &log ) : m_log( log ) {
		m_nCode = 0;
	}

	void setNextCode( int nCode ) {m_nCode = nCode;}

	int overflow( int ch ) {
		Lock sync( m_mutex );

		if( ch != '\n' )
			m_sLine += ch;
		else {
			if( m_nCode ) {
				log.put( m_nCode, m_sLine.c_str() );			
				m_nCode = 0;
			}
			else
				log.put( m_sLine.c_str() );
			m_sLine = "";
		}
		return ch;
	}
};

Log::Log() : ostream( m_pStreamBuf ) 
{
	m_pStreamBuf = new LogStreamBuf( *this );

	rdbuf( m_pStreamBuf );
	
	m_logger = new ErrStreamLogger();
}

void Log::put( int nCode )
{
	m_logger->put( nCode, "" );
}

void Log::put( int nCode, const char *pszParamFmt, ... )
{
	char str[ 1024 ];

	va_list vlist;
	va_start( vlist, pszParamFmt );

	vsnprintf( str, sizeof( str ), pszParamFmt, vlist );

	m_logger->put( nCode, str );

	va_end( vlist );
}

void Log::put( const char *pszFmt, ... )
{
	char str[ 1024 ];

	va_list vlist;
	va_start( vlist, pszFmt );

	vsnprintf( str, sizeof( str ), pszFmt, vlist );

	m_logger->put( str );

	va_end( vlist );
}

void Log::set( RefHandle<Logger> logger )
{
	m_logger = logger;
}

Log &Log::operator()( int nCode ) 
{
	setCode( nCode );
	return *this;
}

void Log::setCode( int nCode ) 
{
	static_cast<LogStreamBuf *>( m_pStreamBuf ) -> setNextCode( nCode );
}

Log log;

}
