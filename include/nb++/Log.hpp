/**
   These log classe are ment as a basic log system, containing both the creation 
   and the output formatter.

   To make the usage easy, the basic log class have one instance, and only one, 
   that will be used in all extern modules. It would have been nice with a factory 
   class, but this would have made the syntax more clumsy.

   Default, the output of the logger ens up in the stderr stream, as console output
   but by registering a new logger it can be put anywere, from file to syslog.

   The log file format, is in plain text :

   timestamp;code;param=text;param=text\n

   The definition file :

   code, level, "text to be interpolated" \n

   When using codes, it is possible to transfere one or several parameters.
   The parameters have names, and are separeted by tabulaters, like this :

   MODULE=local\tLINE=12

   This can be compined with the printf notation, so the end result would look 
   something like this :

   log.put( ERR_MOD, "MODUL=%s\tLINE=%d", __FILE__, __LINE__);

   This more and less directly end like this in the log file, and only by expanding it
   using the LogDefinition class will it be readable, or by using the "tl" (translate log) 
   utility. 

   The definition file defines the codes, and the loglevel. And then it have the
   possibility of interpolating the info. from the user supplyed parameters. A 
   definition entry could look like this :

   ERR_MOD, ERROR, "Error in line %LINE in the module %MODULE"

   From this we can merge the final result to view the log file, and we can 
   generate a prototype header file containing the values of the codes.

   The type can be the following values :

   INFO
   WARNING
   ERROR
   FATAL

   Only the log shower will use there values ... yet !

   If the symbols are needed in the source code, you could use the "logdef_cnv.py", to 
   generate the "log_code.h" header file, that contains the propper symbol definitions
   for the log entries.
*/
#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <string>
#include <fstream>

#include <map>
#include <string>

#include <nb++/RefCount.hpp>
#include <nb++/Mutex.hpp>
#include <nb++/Time.hpp>

namespace nbpp {

	using namespace std;

class LogDefinition;

class LogLine {
	Time m_timestamp;
	string m_sText;
	int m_nLevel;
	int m_nCode;
public:
	LogLine( const LogDefinition &def, const string &szLine );

	int getLevel( void ) const {return m_nLevel;}
	string getText( void ) const {return m_sText;}
	int getCode( void ) const {return m_nCode;}
	const Time &getTimestamp( void ) const {return m_timestamp;}
};
	
class LogDefinition {
public:
	enum Level {
		Info,
		Warning,
		Error,
		Fatal
	};

	LogDefinition( const string &sDefFile = "logcode.def");

	int loadDefs( const string &sDefFile );
	
	Level getLevel( int nCode ) const;
	string getRawText( int nCode ) const;

	LogLine parseLine( const string &sLine ) const;
private:
	struct LineDef {
		Level l;
		string sLine;
	};
	typedef map<int, LineDef> codes_t;
	codes_t m_codes;
};

class Logger : public RefCounted {
public:
	virtual ~Logger() {}

	virtual void put( int nCode, const string &sParam ) = 0; 
	virtual void put( const string &sLine ) = 0;
};

class NullLogger : public Logger
{
public:
	void put( int nCode, const string &sParam ) {}
	void put( const string &sLine ) {}
};

class FileLogger : public Logger
{
	ofstream m_os;
	bool m_bParanoid;
	string m_sFName;
	unsigned m_nLineCount;
	Mutex m_mutex;
public:
	FileLogger( const string &sFName, bool bParanoid = true );

	void put( int nCode, const string &sParam );
	void put( const string &sLine );
};

class Log : public ostream {
	RefHandle<Logger> m_logger;
	streambuf *m_pStreamBuf;
public:
	typedef void (*func)( Log &, int nCode );

	Log( void );

	void put( int nCode );
	void put( int nCode, const char *pszParamFmt, ... );
	void put( const char *pszFmt, ... );

	Log &operator()( int nCode );

	void setCode( int nCode );  // Set default code, for the next log line (streams)
	Log &operator=( int nCode ) {setCode( nCode ); return *this;}

	void set( RefHandle<Logger> logger );
};

extern Log log;

// Code line log
#define NBLOG(code,values) {nbpp::log(code)<<"line="<<__LINE__<<";file="<<__FILE__<<";"<<values<<endl;}
}

#endif
